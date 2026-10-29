/**
 ******************************************************************************
 * @file    ResultScreen.cpp
 * @brief   Result feedback screen implementation.
 ******************************************************************************
 */

#include "gui/screens/ResultScreen.hpp"
#include "gui/screens/ScreenManager.hpp"
#include "gui/theme/Theme.hpp"
#include "gui/Assets.hpp"

using namespace SDK::GUI;

ResultScreen::ResultScreen(Model& model)
    : Screen(model)
{
}

ResultScreen::~ResultScreen()
{
    lv_anim_delete(this, &ResultScreen::animExecCb);
}

void ResultScreen::build()
{
    using F = Theme::Font;

    mRing   = std::make_unique<Widgets::TimerRing>(mRoot, Color::CHARTREUSE);
    mIcon   = Theme::image(mRoot, &img_circletick_50x50, 95, 36);
    mStatus = Theme::label(mRoot, F::SemiBold30, "Saved", 20, 99, 200);
    mDesc   = Theme::label(mRoot, F::Regular18, "", 20, 148, 200);
}

void ResultScreen::onShow()
{
    mFinished = false;
    const auto mode = mModel.getResultMode();

    switch (mode) {
        case Model::ResultMode::SettingSaved:
            mTimeoutMs = 1000;
            mRing->setColor(Color::CHARTREUSE);
            lv_image_set_src(mIcon, &img_circletick_50x50);
            lv_obj_set_pos(mIcon, 95, 58);
            lv_label_set_text(mStatus, "Saved");
            lv_obj_set_pos(mStatus, 20, 128);
            Theme::setHidden(mDesc, true);
            break;

        case Model::ResultMode::WorkoutSaved:
            mTimeoutMs = 2000;
            mRing->setColor(Color::CHARTREUSE);
            lv_image_set_src(mIcon, &img_circletick_50x50);
            lv_obj_set_pos(mIcon, 95, 36);
            lv_label_set_text(mStatus, "Saved");
            lv_obj_set_pos(mStatus, 20, 99);
            lv_label_set_text(mDesc, "Activity has\nbeen saved");
            Theme::setHidden(mDesc, false);
            break;

        case Model::ResultMode::WorkoutDiscarded:
            mTimeoutMs = 2000;
            mRing->setColor(Color::CRIMSON);
            lv_image_set_src(mIcon, &img_circlecross_50x50);
            lv_obj_set_pos(mIcon, 95, 36);
            lv_label_set_text(mStatus, "Discarded");
            lv_obj_set_pos(mStatus, 20, 99);
            lv_label_set_text(mDesc, "Activity has\nbeen deleted");
            Theme::setHidden(mDesc, false);
            break;
    }

    mRing->setProgress(0.0f);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, this);
    lv_anim_set_exec_cb(&a, &ResultScreen::animExecCb);
    lv_anim_set_completed_cb(&a, &ResultScreen::animReadyCb);
    lv_anim_set_values(&a, 0, static_cast<int32_t>(mTimeoutMs));
    lv_anim_set_duration(&a, mTimeoutMs);
    lv_anim_start(&a);
}

void ResultScreen::onHide()
{
    lv_anim_delete(this, &ResultScreen::animExecCb);
}

void ResultScreen::onKey(uint8_t code)
{
    switch (code) {
        case Btn::R1:
        case Btn::R2:
            finish();
            break;

        default:
            break;
    }
}

void ResultScreen::animExecCb(void* var, int32_t value)
{
    auto* self = static_cast<ResultScreen*>(var);
    if (self->mTimeoutMs > 0) {
        self->mRing->setProgress(static_cast<float>(value) / static_cast<float>(self->mTimeoutMs));
    }
}

void ResultScreen::animReadyCb(lv_anim_t* a)
{
    static_cast<ResultScreen*>(a->var)->finish();
}

void ResultScreen::finish()
{
    if (mFinished) return;
    mFinished = true;

    lv_anim_delete(this, &ResultScreen::animExecCb);

    if (mModel.getResultMode() == Model::ResultMode::WorkoutSaved) {
        ScreenManager::instance().goTo(ScreenId::Summary);
    } else {
        ScreenManager::instance().goTo(ScreenId::Main);
    }
}

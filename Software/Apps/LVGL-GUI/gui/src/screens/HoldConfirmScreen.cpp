/**
 ******************************************************************************
 * @file    HoldConfirmScreen.cpp
 * @brief   Hold-to-confirm screen implementation (modeled after RunLVGL).
 ******************************************************************************
 */

#include "gui/screens/HoldConfirmScreen.hpp"
#include "gui/screens/ScreenManager.hpp"
#include "gui/theme/Theme.hpp"
#include "gui/Assets.hpp"

using namespace SDK::GUI;

HoldConfirmScreen::HoldConfirmScreen(Model& model)
    : Screen(model)
{
}

HoldConfirmScreen::~HoldConfirmScreen()
{
    lv_anim_delete(this, nullptr);
}

void HoldConfirmScreen::build()
{
    mMode = mModel.getHoldConfirmMode();
    const bool finish = (mMode == Model::HoldConfirmMode::Finish);

    const uint32_t color = finish ? Color::CHARTREUSE : Color::RED;
    mRing = std::make_unique<Widgets::TimerRing>(mRoot, color);

    mButtons = std::make_unique<Widgets::Buttons>(mRoot);
    mButtons->set(Widgets::Buttons::NONE,
                  Widgets::Buttons::NONE,
                  finish ? Widgets::Buttons::GREEN : Widgets::Buttons::RED,
                  Widgets::Buttons::NONE);

    Theme::imageTinted(mRoot, &img_tickgreen_22x17, 186, 60, color);
    Theme::label(mRoot, Theme::Font::Medium18,
                 finish ? "Hold to\nFinish" : "Hold to\nDiscard",
                 53, 67, 133);
    mNumber = Theme::label(mRoot, Theme::Font::SemiBold60, "3", 102, 103, 36);
}

void HoldConfirmScreen::onShow()
{
    mFired = false;
    mShown = 3;
    mRing->setProgress(0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, this);
    lv_anim_set_values(&a, 0, static_cast<int32_t>(kHoldMs));
    lv_anim_set_duration(&a, kHoldMs);
    lv_anim_set_exec_cb(&a, &HoldConfirmScreen::animExecCb);
    lv_anim_set_completed_cb(&a, &HoldConfirmScreen::animReadyCb);
    lv_anim_start(&a);
}

void HoldConfirmScreen::onHide()
{
    lv_anim_delete(this, nullptr);
}

void HoldConfirmScreen::onKey(uint8_t code)
{
    // Releasing R1 before the countdown completes (or pressing R2 Back) returns to Pause menu
    if (code == Btn::R1_RELEASE || code == Btn::R2) {
        cancel();
    }
}

void HoldConfirmScreen::cancel()
{
    if (mFired) {
        return;
    }
    lv_anim_delete(this, nullptr);
    ScreenManager::instance().goTo(ScreenId::Pause);
}

void HoldConfirmScreen::animExecCb(void* var, int32_t value)
{
    auto* self = static_cast<HoldConfirmScreen*>(var);
    if (value < 0) {
        value = 0;
    }
    self->mRing->setProgress(static_cast<uint32_t>(value) * 1000u / kHoldMs);

    // 0..kHoldMs maps onto 3 -> 2 -> 1
    uint32_t number = 3u - (static_cast<uint32_t>(value) / (kHoldMs / 3u));
    if (number < 1u) {
        number = 1u;
    }
    self->setCountdown(number);
}

void HoldConfirmScreen::animReadyCb(lv_anim_t* a)
{
    auto* self = static_cast<HoldConfirmScreen*>(a->var);
    if (self->mFired) {
        return;
    }
    self->mFired = true;

    self->mModel.setMainCursor(0);
    if (self->mMode == Model::HoldConfirmMode::Finish) {
        self->mModel.stopTrack(false);
        self->mModel.setResultMode(Model::ResultMode::WorkoutSaved);
    } else {
        self->mModel.stopTrack(true);
        self->mModel.setResultMode(Model::ResultMode::WorkoutDiscarded);
    }
    ScreenManager::instance().goTo(ScreenId::Result);
}

void HoldConfirmScreen::setCountdown(uint32_t number)
{
    if (number != mShown) {
        mShown = number;
        lv_label_set_text_fmt(mNumber, "%u", static_cast<unsigned>(number));
    }
}

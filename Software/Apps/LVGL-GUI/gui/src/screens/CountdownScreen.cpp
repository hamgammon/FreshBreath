/**
 ******************************************************************************
 * @file    CountdownScreen.cpp
 * @brief   5-second preparation countdown screen implementation.
 ******************************************************************************
 */

#include "gui/screens/CountdownScreen.hpp"
#include "gui/screens/ScreenManager.hpp"
#include "gui/theme/Theme.hpp"
#include "gui/Assets.hpp"
#include "BreathPatterns.hpp"

#include <cstdio>

using namespace SDK::GUI;

CountdownScreen::CountdownScreen(Model& model)
    : Screen(model)
{
}

CountdownScreen::~CountdownScreen()
{
    lv_anim_delete(this, &CountdownScreen::animExecCb);
}

void CountdownScreen::build()
{
    using F = Theme::Font;

    mRing    = std::make_unique<Widgets::TimerRing>(mRoot, Color::TEAL);
    mButtons = std::make_unique<Widgets::Buttons>(mRoot);
    mButtons->set(Widgets::Buttons::NONE,
                  Widgets::Buttons::NONE,
                  Widgets::Buttons::AMBER,
                  Widgets::Buttons::WHITE);

    Theme::image(mRoot, &img_tickgreen_22x17, 186, 60);
    Theme::image(mRoot, &img_crosswhite_17x17, 187, 163);

    Theme::hline(mRoot, 35, 81, 170, Color::GRAY_DARK);
    Theme::hline(mRoot, 35, 113, 170, Color::GRAY_DARK);

    mPattern = Theme::label(mRoot, F::SemiBold20, "", 35, 55, 145, LV_TEXT_ALIGN_CENTER, Color::TEAL);
    mRatio   = Theme::label(mRoot, F::Medium18,   "", 35, 87, 145, LV_TEXT_ALIGN_CENTER, Color::WHITE);
    mTarget  = Theme::label(mRoot, F::Italic18,   "Get Ready", 35, 119, 145, LV_TEXT_ALIGN_CENTER, Color::GRAY);
    mCount   = Theme::label(mRoot, F::SemiBold60, "5", 70, 142, 100);
}

void CountdownScreen::onShow()
{
    mStarted        = false;
    mLastShownCount = -1;

    const auto& s = mModel.getSettings();
    const auto& p = FreshBreath::PatternRegistry::getPattern(s.selectedPatternIndex);

    lv_label_set_text(mPattern, p.displayName);

    char timeBuf[24];
    if (s.targetDurationSec == 0) {
        std::snprintf(timeBuf, sizeof(timeBuf), "Open Session");
    } else {
        std::snprintf(timeBuf, sizeof(timeBuf), "%u min",
                      static_cast<unsigned>(s.targetDurationSec / 60));
    }
    lv_label_set_text(mRatio, timeBuf);
    lv_label_set_text(mTarget, "Get Ready");

    mRing->setRemaining(1000);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, this);
    lv_anim_set_exec_cb(&a, &CountdownScreen::animExecCb);
    lv_anim_set_completed_cb(&a, &CountdownScreen::animReadyCb);
    lv_anim_set_values(&a, 0, static_cast<int32_t>(kTimeoutMs));
    lv_anim_set_duration(&a, kTimeoutMs);
    lv_anim_start(&a);
}

void CountdownScreen::onHide()
{
    lv_anim_delete(this, &CountdownScreen::animExecCb);
}

void CountdownScreen::onKey(uint8_t code)
{
    switch (code) {
        case Btn::R1: // Skip countdown & start immediately
            startBreathing();
            break;

        case Btn::R2: // Back -> cancel & return to MainScreen
            lv_anim_delete(this, &CountdownScreen::animExecCb);
            ScreenManager::instance().goTo(ScreenId::Main);
            break;

        default:
            break;
    }
}

void CountdownScreen::animExecCb(void* var, int32_t value)
{
    auto* self = static_cast<CountdownScreen*>(var);
    if (value < 0) value = 0;
    const float rem = 1.0f - static_cast<float>(value) / static_cast<float>(kTimeoutMs);
    self->mRing->setRemaining(1000u - (static_cast<uint32_t>(value) * 1000u / kTimeoutMs));

    int32_t shown = static_cast<int32_t>(rem * kCountTicks) + 1;
    if (shown > kCountTicks) shown = kCountTicks;
    if (shown < 1) shown = 1;

    if (shown != self->mLastShownCount) {
        self->mLastShownCount = shown;
        lv_label_set_text_fmt(self->mCount, "%d", static_cast<int>(shown));
    }
}

void CountdownScreen::animReadyCb(lv_anim_t* a)
{
    auto* self = static_cast<CountdownScreen*>(a->var);
    self->startBreathing();
}

void CountdownScreen::startBreathing()
{
    if (mStarted) return;
    mStarted = true;

    lv_anim_delete(this, &CountdownScreen::animExecCb);

    const auto& s = mModel.getSettings();
    mModel.startTrack(s.selectedPatternIndex, s.targetDurationSec);
    ScreenManager::instance().goTo(ScreenId::Breath);
}

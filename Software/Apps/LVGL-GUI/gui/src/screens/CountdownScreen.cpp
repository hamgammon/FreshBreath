/**
 ******************************************************************************
 * @file    CountdownScreen.cpp
 * @brief   10-second preparation countdown screen implementation.
 ******************************************************************************
 */

#include "gui/screens/CountdownScreen.hpp"
#include "gui/screens/ScreenManager.hpp"
#include "gui/theme/Theme.hpp"
#include "BreathPatterns.hpp"

#include <cstdio>
#include <algorithm>

CountdownScreen::CountdownScreen(Model& model)
    : Screen(model)
{
}

CountdownScreen::~CountdownScreen()
{
    if (mTimer) {
        lv_timer_delete(mTimer);
        mTimer = nullptr;
    }
}

void CountdownScreen::build()
{
    mButtons = std::make_unique<SDK::LVGL::Buttons>(mRoot);
    mButtons->set(SDK::LVGL::Buttons::RED,     // L1: Cancel
                  SDK::LVGL::Buttons::NONE,    // L2: None
                  SDK::LVGL::Buttons::GREEN,   // R1: Skip & Start
                  SDK::LVGL::Buttons::NONE);   // R2: None

    // Background track ring
    Theme::arc(mRoot, 120, 120, 95, 6, 0, 360, SDK::GUI::Color::TEAL_DARK, false);

    // Active animated countdown arc
    mCountdownArc = Theme::arc(mRoot, 120, 120, 95, 8, 0, 360, SDK::GUI::Color::CYAN_PURE, true);

    // Header
    mTitleLabel = Theme::label(mRoot, Theme::Font::SemiBold20, "GET READY", 0, 24, 240,
                               LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::CYAN_PURE);

    // Selected Pattern Name
    const Settings& s = mModel.getSettings();
    const FreshBreath::BreathPattern& p = FreshBreath::PatternRegistry::getPattern(s.selectedPatternIndex);
    mPatternLabel = Theme::label(mRoot, Theme::Font::Regular14, p.displayName, 0, 48, 240,
                                 LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::AQUA);

    // Center Big Countdown Number
    mCountLabel = Theme::label(mRoot, Theme::Font::SemiBold60, "10", 0, 80, 240,
                               LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::WHITE);

    // Subtext prompt
    mHintLabel = Theme::label(mRoot, Theme::Font::Regular14, "Settle in and relax...", 0, 154, 240,
                              LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::GRAY);

    // Nav prompt
    mNavLabel = Theme::label(mRoot, Theme::Font::Regular14, "L1: Cancel • R1: Skip", 0, 196, 240,
                             LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::GRAY);
}

void CountdownScreen::onShow()
{
    mSecondsLeft = 10;
    updateCountdown();

    if (!mTimer) {
        mTimer = lv_timer_create(onTimerTick, 1000, this);
    } else {
        lv_timer_reset(mTimer);
    }
}

void CountdownScreen::onHide()
{
    if (mTimer) {
        lv_timer_delete(mTimer);
        mTimer = nullptr;
    }
}

void CountdownScreen::onTimerTick(lv_timer_t* timer)
{
    auto* self = static_cast<CountdownScreen*>(lv_timer_get_user_data(timer));
    if (!self) return;

    self->mSecondsLeft--;
    if (self->mSecondsLeft <= 0) {
        if (self->mTimer) {
            lv_timer_delete(self->mTimer);
            self->mTimer = nullptr;
        }
        self->startBreathing();
    } else {
        self->updateCountdown();
    }
}

void CountdownScreen::updateCountdown()
{
    if (mCountLabel) {
        char buf[8];
        std::snprintf(buf, sizeof(buf), "%d", mSecondsLeft);
        lv_label_set_text(mCountLabel, buf);
    }

    if (mCountdownArc) {
        int32_t progress = (static_cast<int32_t>(mSecondsLeft) * 360) / 10;
        progress = std::max<int32_t>(0, std::min<int32_t>(360, progress));
        Theme::setArc(mCountdownArc, 0, progress);
    }
}

void CountdownScreen::startBreathing()
{
    const Settings& s = mModel.getSettings();
    mModel.startTrack(s.selectedPatternIndex, s.targetDurationSec);
    ScreenManager::instance().goTo(ScreenId::Breath);
}

void CountdownScreen::onKey(uint8_t code)
{
    switch (code) {
        case SDK::GUI::Button::R1: // Skip countdown and start immediately
            if (mTimer) {
                lv_timer_delete(mTimer);
                mTimer = nullptr;
            }
            startBreathing();
            break;

        case SDK::GUI::Button::L1: // Cancel and return to main screen
            if (mTimer) {
                lv_timer_delete(mTimer);
                mTimer = nullptr;
            }
            ScreenManager::instance().goTo(ScreenId::Main);
            break;

        default:
            break;
    }
}

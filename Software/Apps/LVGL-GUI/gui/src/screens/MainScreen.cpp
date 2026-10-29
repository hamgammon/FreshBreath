/**
 ******************************************************************************
 * @file    MainScreen.cpp
 * @brief   Main WheelMenu screen implementation for FreshBreath.
 ******************************************************************************
 */

#include "gui/screens/MainScreen.hpp"
#include "gui/screens/ScreenManager.hpp"
#include "gui/theme/Theme.hpp"
#include "gui/Assets.hpp"
#include "BreathPatterns.hpp"

#include <cstdio>

using namespace SDK::GUI;

MainScreen::MainScreen(Model& model)
    : Screen(model)
{
}

void MainScreen::build()
{
    populateItems();

    mMenu    = std::make_unique<Widgets::WheelMenu>(mRoot, mItems, kItemCount);
    mTitle   = std::make_unique<Widgets::Title>(mRoot, "FRESHBREATH");
    mButtons = std::make_unique<Widgets::Buttons>(mRoot);
    mButtons->set(Widgets::Buttons::NONE,
                  Widgets::Buttons::NONE,
                  Widgets::Buttons::AMBER,
                  Widgets::Buttons::WHITE);
}

void MainScreen::onShow()
{
    if (mModel.getTrackState() == Track::State::ACTIVE) {
        ScreenManager::instance().goTo(ScreenId::Breath);
        return;
    }
    if (mModel.getTrackState() == Track::State::PAUSED) {
        ScreenManager::instance().goTo(ScreenId::Pause);
        return;
    }

    populateItems();
    mMenu->refresh();
    mMenu->select(mModel.getMainCursor());
}

void MainScreen::populateItems()
{
    using Style = Widgets::WheelMenu::Style;

    const auto& s = mModel.getSettings();
    const auto& p = FreshBreath::PatternRegistry::getPattern(s.selectedPatternIndex);
    const auto& sum = mModel.getSummary();

    if (s.targetDurationSec == 0) {
        std::snprintf(mTimeTip, sizeof(mTimeTip), "Open");
    } else {
        std::snprintf(mTimeTip, sizeof(mTimeTip), "%u min",
                      static_cast<unsigned>(s.targetDurationSec / 60));
    }

    std::snprintf(mStartTip, sizeof(mStartTip), "%s  %s", p.displayName, mTimeTip);
    std::snprintf(mPatternTip, sizeof(mPatternTip), "%s (%s)", p.displayName, p.ratioLabel);
    std::snprintf(mAlertModeTip, sizeof(mAlertModeTip), "%s",
                  s.vibrateOnly ? "Vibration Only" : "Sound & Vibrate");

    const bool hasSummary = (sum.duration > 0);
    if (hasSummary) {
        unsigned mins = static_cast<unsigned>(sum.duration / 60);
        unsigned secs = static_cast<unsigned>(sum.duration % 60);
        std::snprintf(mSummaryTip, sizeof(mSummaryTip), "%02u:%02u  %u cyc",
                      mins, secs, static_cast<unsigned>(sum.cycles));
    } else {
        std::snprintf(mSummaryTip, sizeof(mSummaryTip), "No workouts yet");
    }

    // 0: Start
    mItems[ITEM_START]            = {};
    mItems[ITEM_START].style      = Style::Tip;
    mItems[ITEM_START].text       = "Start";
    mItems[ITEM_START].centerFont = &poppins_semibold_30;
    mItems[ITEM_START].tip        = mStartTip;
    mItems[ITEM_START].tipColor   = Color::TEAL;

    // 1: Pattern
    mItems[ITEM_PATTERN]            = {};
    mItems[ITEM_PATTERN].style      = Style::Tip;
    mItems[ITEM_PATTERN].text       = "Pattern";
    mItems[ITEM_PATTERN].centerFont = &poppins_semibold_25;
    mItems[ITEM_PATTERN].tip        = mPatternTip;
    mItems[ITEM_PATTERN].tipColor   = Color::TEAL;

    // 2: Time
    mItems[ITEM_TIME]            = {};
    mItems[ITEM_TIME].style      = Style::Tip;
    mItems[ITEM_TIME].text       = "Time";
    mItems[ITEM_TIME].centerFont = &poppins_semibold_25;
    mItems[ITEM_TIME].tip        = mTimeTip;
    mItems[ITEM_TIME].tipColor   = Color::TEAL;

    // 3: Alert Mode (Sound & Vibrate / Vibration Only)
    mItems[ITEM_ALERT_MODE]            = {};
    mItems[ITEM_ALERT_MODE].style      = Style::Tip;
    mItems[ITEM_ALERT_MODE].text       = "Alert Mode";
    mItems[ITEM_ALERT_MODE].centerFont = &poppins_semibold_25;
    mItems[ITEM_ALERT_MODE].tip        = mAlertModeTip;
    mItems[ITEM_ALERT_MODE].tipColor   = Color::TEAL;

    // 4: Summary
    mItems[ITEM_SUMMARY]            = {};
    mItems[ITEM_SUMMARY].style      = Style::Tip;
    mItems[ITEM_SUMMARY].text       = "Summary";
    mItems[ITEM_SUMMARY].centerFont = &poppins_semibold_25;
    mItems[ITEM_SUMMARY].tip        = mSummaryTip;
    mItems[ITEM_SUMMARY].tipColor   = hasSummary ? Color::TEAL : Color::GRAY;
}

void MainScreen::onKey(uint8_t code)
{
    switch (code) {
        case Btn::L1:
            mMenu->prev();
            break;

        case Btn::L2:
            mMenu->next();
            break;

        case Btn::R1: {
            const uint16_t sel = mMenu->selected();
            mModel.setMainCursor(sel);

            switch (sel) {
                case ITEM_START:
                    ScreenManager::instance().goTo(ScreenId::Countdown);
                    break;

                case ITEM_PATTERN:
                    ScreenManager::instance().goTo(ScreenId::PatternSelect);
                    break;

                case ITEM_TIME:
                    ScreenManager::instance().goTo(ScreenId::DurationSelect);
                    break;

                case ITEM_ALERT_MODE: {
                    Settings s = mModel.getSettings();
                    s.vibrateOnly = !s.vibrateOnly;
                    mModel.saveSettings(s);
                    populateItems();
                    mMenu->refresh();
                } break;

                case ITEM_SUMMARY:
                    if (mModel.getSummary().duration > 0) {
                        ScreenManager::instance().goTo(ScreenId::Summary);
                    }
                    break;

                default:
                    break;
            }
        } break;

        case Btn::R2:
            mModel.exitApp();
            break;

        default:
            break;
    }
}

void MainScreen::onSettingsUpdated(const Settings& settings)
{
    (void)settings;
    populateItems();
    if (mMenu) {
        mMenu->refresh();
    }
}

void MainScreen::onSummaryUpdated(const ActivitySummary& summary)
{
    (void)summary;
    populateItems();
    if (mMenu) {
        mMenu->refresh();
    }
}

void MainScreen::onTrackStateChanged(Track::State state)
{
    if (state == Track::State::ACTIVE) {
        ScreenManager::instance().goTo(ScreenId::Breath);
    } else if (state == Track::State::PAUSED) {
        ScreenManager::instance().goTo(ScreenId::Pause);
    }
}

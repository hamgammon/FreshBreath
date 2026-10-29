/**
 ******************************************************************************
 * @file    MainScreen.cpp
 * @brief   FreshBreath main home screen implementation.
 ******************************************************************************
 */

#include "gui/screens/MainScreen.hpp"
#include "gui/screens/ScreenManager.hpp"
#include "gui/theme/Theme.hpp"
#include "BreathPatterns.hpp"

#include <cstdio>

MainScreen::MainScreen(Model& model)
    : Screen(model)
{
}

void MainScreen::build()
{
    mButtons = std::make_unique<SDK::LVGL::Buttons>(mRoot);
    mButtons->set(SDK::LVGL::Buttons::WHITE,   // L1: Exit / Summary
                  SDK::LVGL::Buttons::AMBER,   // L2: Pattern
                  SDK::LVGL::Buttons::GREEN,   // R1: Start
                  SDK::LVGL::Buttons::AMBER);  // R2: Duration

    // App header
    mTitleLabel = Theme::label(mRoot, Theme::Font::SemiBold20, "FRESH BREATH", 0, 15, 240,
                               LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::CYAN_PURE);

    mTimeLabel = Theme::label(mRoot, Theme::Font::Regular14, "12:00", 0, 38, 240,
                              LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::GRAY);

    // Selected Pattern Box
    mPatternCard = Theme::container(mRoot, 20, 62, 200, 72);
    lv_obj_set_style_bg_color(mPatternCard, Theme::rgb(SDK::GUI::Color::TEAL_DARK), 0);
    lv_obj_set_style_bg_opa(mPatternCard, LV_OPA_50, 0);
    lv_obj_set_style_radius(mPatternCard, 8, 0);

    Theme::label(mPatternCard, Theme::Font::Regular14, "PATTERN [L2]", 0, 6, 200,
                 LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::AQUA);

    mPatternName = Theme::label(mPatternCard, Theme::Font::SemiBold20, "Box 4-4-4-4", 0, 24, 200,
                                LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::WHITE);

    mPatternRatio = Theme::label(mPatternCard, Theme::Font::Regular14, "4s : 4s : 4s : 4s", 0, 48, 200,
                                 LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::GRAY);

    // Target Duration Box
    lv_obj_t* durCard = Theme::container(mRoot, 20, 140, 200, 45);
    lv_obj_set_style_bg_color(durCard, Theme::rgb(SDK::GUI::Color::GRAY_DARK), 0);
    lv_obj_set_style_bg_opa(durCard, LV_OPA_40, 0);
    lv_obj_set_style_radius(durCard, 8, 0);

    Theme::label(durCard, Theme::Font::Regular14, "DURATION [R2]", 0, 4, 200,
                 LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::YELLOW_DARK);

    mDurationLabel = Theme::label(durCard, Theme::Font::SemiBold20, "5 min", 0, 20, 200,
                                  LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::WHITE);

    // Start prompt
    mHintLabel = Theme::label(mRoot, Theme::Font::SemiBold20, "Press R1 to Start", 0, 198, 240,
                              LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::GREEN);

    updateCards();
    updateTimeLabel();
}

void MainScreen::onShow()
{
    updateCards();
    updateTimeLabel();
}

void MainScreen::onKey(uint8_t code)
{
    switch (code) {
        case SDK::GUI::Button::R1: { // Start breathing session
            const Settings& s = mModel.getSettings();
            mModel.startTrack(s.selectedPatternIndex, s.targetDurationSec);
            ScreenManager::instance().goTo(ScreenId::Breath);
        } break;

        case SDK::GUI::Button::L2: // Change pattern
            ScreenManager::instance().goTo(ScreenId::PatternSelect);
            break;

        case SDK::GUI::Button::R2: // Change duration
            ScreenManager::instance().goTo(ScreenId::DurationSelect);
            break;

        case SDK::GUI::Button::L1: // Summary if available, else exit
            if (mModel.getSummary().duration > 0) {
                ScreenManager::instance().goTo(ScreenId::Summary);
            } else {
                mModel.exitApp();
            }
            break;

        default:
            break;
    }
}

void MainScreen::updateCards()
{
    const Settings& s = mModel.getSettings();
    const FreshBreath::BreathPattern& p = FreshBreath::PatternRegistry::getPattern(s.selectedPatternIndex);

    if (mPatternName) {
        lv_label_set_text(mPatternName, p.displayName);
    }
    if (mPatternRatio) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%u : %u : %u : %u",
                      p.inhaleSec, p.holdInSec, p.exhaleSec, p.holdOutSec);
        lv_label_set_text(mPatternRatio, buf);
    }
    if (mDurationLabel) {
        if (s.targetDurationSec == 0) {
            lv_label_set_text(mDurationLabel, "Open (No Limit)");
        } else {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%u min", s.targetDurationSec / 60);
            lv_label_set_text(mDurationLabel, buf);
        }
    }
}

void MainScreen::updateTimeLabel()
{
    if (!mTimeLabel) return;
    const std::tm& t = mModel.getTime();
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%02d:%02d", t.tm_hour, t.tm_min);
    lv_label_set_text(mTimeLabel, buf);
}

void MainScreen::onTimeUpdated(const std::tm& time)
{
    (void)time;
    updateTimeLabel();
}

void MainScreen::onBatteryUpdated(uint8_t level)
{
    (void)level;
}

void MainScreen::onSettingsUpdated(const Settings& settings)
{
    (void)settings;
    updateCards();
}

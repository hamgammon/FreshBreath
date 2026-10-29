/**
 ******************************************************************************
 * @file    DurationSelectScreen.cpp
 * @brief   Screen to choose session target duration implementation.
 ******************************************************************************
 */

#include "gui/screens/DurationSelectScreen.hpp"
#include "gui/screens/ScreenManager.hpp"
#include "gui/theme/Theme.hpp"

#include <cstdio>

const uint16_t DurationSelectScreen::kDurations[DurationSelectScreen::kOptionCount] = {
    60,   // 1 min
    180,  // 3 min
    240,  // 4 min
    300,  // 5 min
    600,  // 10 min
    900,  // 15 min
    1200, // 20 min
    0     // Open
};

DurationSelectScreen::DurationSelectScreen(Model& model)
    : Screen(model)
{
    uint16_t currentDur = model.getSettings().targetDurationSec;
    for (size_t i = 0; i < kOptionCount; ++i) {
        if (kDurations[i] == currentDur) {
            mCurrentIndex = i;
            break;
        }
    }
}

void DurationSelectScreen::build()
{
    mButtons = std::make_unique<SDK::LVGL::Buttons>(mRoot);
    mButtons->set(SDK::LVGL::Buttons::WHITE,   // L1: Prev
                  SDK::LVGL::Buttons::WHITE,   // L2: Next
                  SDK::LVGL::Buttons::GREEN,   // R1: Select
                  SDK::LVGL::Buttons::RED);    // R2: Cancel

    mTitleLabel = Theme::label(mRoot, Theme::Font::Regular14, "SELECT DURATION", 0, 20, 240,
                               LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::GRAY);

    lv_obj_t* card = Theme::container(mRoot, 20, 55, 200, 120);
    lv_obj_set_style_bg_color(card, Theme::rgb(SDK::GUI::Color::GRAY_DARK), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_40, 0);
    lv_obj_set_style_radius(card, 10, 0);

    mDurationLabel = Theme::label(card, Theme::Font::SemiBold40, "", 0, 20, 200,
                                  LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::CYAN_PURE);

    mSubLabel = Theme::label(card, Theme::Font::Regular16, "", 0, 75, 200,
                             LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::WHITE);

    mNavLabel = Theme::label(mRoot, Theme::Font::Regular14, "L1: Up • L2: Down • R1: OK", 0, 195, 240,
                             LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::GRAY);

    updateView();
}

void DurationSelectScreen::onShow()
{
    uint16_t currentDur = mModel.getSettings().targetDurationSec;
    for (size_t i = 0; i < kOptionCount; ++i) {
        if (kDurations[i] == currentDur) {
            mCurrentIndex = i;
            break;
        }
    }
    updateView();
}

void DurationSelectScreen::onKey(uint8_t code)
{
    switch (code) {
        case SDK::GUI::Button::L2: // Next duration
            mCurrentIndex = (mCurrentIndex + 1) % kOptionCount;
            updateView();
            break;

        case SDK::GUI::Button::L1: // Previous duration
            mCurrentIndex = (mCurrentIndex + kOptionCount - 1) % kOptionCount;
            updateView();
            break;

        case SDK::GUI::Button::R1: { // Confirm
            Settings s = mModel.getSettings();
            s.targetDurationSec = kDurations[mCurrentIndex];
            mModel.saveSettings(s);
            ScreenManager::instance().goTo(ScreenId::Main);
        } break;

        case SDK::GUI::Button::R2: // Cancel
            ScreenManager::instance().goTo(ScreenId::Main);
            break;

        default:
            break;
    }
}

void DurationSelectScreen::updateView()
{
    if (mTitleLabel) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "SELECT DURATION (%u/%u)",
                      static_cast<unsigned>(mCurrentIndex + 1),
                      static_cast<unsigned>(kOptionCount));
        lv_label_set_text(mTitleLabel, buf);
    }

    uint16_t dur = kDurations[mCurrentIndex];
    if (dur == 0) {
        if (mDurationLabel) lv_label_set_text(mDurationLabel, "Open");
        if (mSubLabel) lv_label_set_text(mSubLabel, "Continuous practice");
    } else {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%u min", dur / 60);
        if (mDurationLabel) lv_label_set_text(mDurationLabel, buf);

        if (mSubLabel) {
            if (dur == 180) {
                lv_label_set_text(mSubLabel, "Quick reset");
            } else if (dur == 300) {
                lv_label_set_text(mSubLabel, "Recommended daily");
            } else if (dur >= 600) {
                lv_label_set_text(mSubLabel, "Deep HRV & Calm");
            } else {
                lv_label_set_text(mSubLabel, "Focused session");
            }
        }
    }
}

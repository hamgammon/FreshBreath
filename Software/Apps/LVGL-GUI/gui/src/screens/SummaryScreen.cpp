/**
 ******************************************************************************
 * @file    SummaryScreen.cpp
 * @brief   Completed breathwork session summary screen implementation.
 ******************************************************************************
 */

#include "gui/screens/SummaryScreen.hpp"
#include "gui/screens/ScreenManager.hpp"
#include "gui/theme/Theme.hpp"
#include "gui/Assets.hpp"

#include <cstdio>

SummaryScreen::SummaryScreen(Model& model)
    : Screen(model)
{
}

void SummaryScreen::build()
{
    mButtons = std::make_unique<SDK::LVGL::Buttons>(mRoot);
    mButtons->set(SDK::LVGL::Buttons::NONE,
                  SDK::LVGL::Buttons::NONE,
                  SDK::LVGL::Buttons::GREEN,   // R1: Done
                  SDK::LVGL::Buttons::NONE);

    // Green tick at top
    Theme::imageTinted(mRoot, &img_tickgreen_22x17, 109, 12, SDK::GUI::Color::GREEN);

    mHeaderLabel = Theme::label(mRoot, Theme::Font::Regular14, "WORKOUT SAVED", 0, 32, 240,
                                LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::AQUA);

    mTitleLabel = Theme::label(mRoot, Theme::Font::SemiBold20, "Breathwork", 0, 48, 240,
                               LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::CYAN_PURE);

    // Summary Card
    lv_obj_t* card = Theme::container(mRoot, 15, 72, 210, 116);
    lv_obj_set_style_bg_color(card, Theme::rgb(SDK::GUI::Color::TEAL_DARK), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_30, 0);
    lv_obj_set_style_radius(card, 8, 0);

    mDurationLabel = Theme::label(card, Theme::Font::SemiBold25, "00:00", 0, 6, 210,
                                  LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::WHITE);

    mCyclesLabel = Theme::label(card, Theme::Font::Regular16, "0 Cycles Completed", 0, 36, 210,
                                LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::AQUA);

    mHrLabel = Theme::label(card, Theme::Font::Regular14, "Avg HR: -- bpm", 0, 64, 210,
                            LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::WHITE);

    mCalLabel = Theme::label(card, Theme::Font::Regular14, "Calories: 0 kcal", 0, 88, 210,
                             LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::GRAY);

    mDonePrompt = Theme::label(mRoot, Theme::Font::SemiBold20, "Press R1 to Finish", 0, 198, 240,
                               LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::GREEN);

    updateView();
}

void SummaryScreen::onShow()
{
    updateView();
}

void SummaryScreen::onKey(uint8_t code)
{
    if (code == SDK::GUI::Button::R1 || code == SDK::GUI::Button::L1) {
        ScreenManager::instance().goTo(ScreenId::Main);
    }
}

void SummaryScreen::updateView()
{
    const ActivitySummary& sum = mModel.getSummary();

    if (mTitleLabel && sum.workoutTitle[0] != '\0') {
        lv_label_set_text(mTitleLabel, sum.workoutTitle);
    }

    if (mDurationLabel) {
        char buf[32];
        unsigned m = static_cast<unsigned>(sum.duration / 60);
        unsigned s = static_cast<unsigned>(sum.duration % 60);
        std::snprintf(buf, sizeof(buf), "%02u:%02u", m, s);
        lv_label_set_text(mDurationLabel, buf);
    }

    if (mCyclesLabel) {
        char buf[48];
        std::snprintf(buf, sizeof(buf), "%u Cycles Completed", sum.cycles);
        lv_label_set_text(mCyclesLabel, buf);
    }

    if (mHrLabel) {
        char buf[64];
        if (sum.hrAvg > 20.0f) {
            std::snprintf(buf, sizeof(buf), "HR Avg: %u • Min: %u",
                          static_cast<unsigned>(sum.hrAvg + 0.5f),
                          static_cast<unsigned>(sum.hrMin + 0.5f));
        } else {
            std::snprintf(buf, sizeof(buf), "HR: --");
        }
        lv_label_set_text(mHrLabel, buf);
    }

    if (mCalLabel) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "Est: %u kcal", static_cast<unsigned>(sum.calories + 0.5f));
        lv_label_set_text(mCalLabel, buf);
    }
}

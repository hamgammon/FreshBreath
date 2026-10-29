/**
 ******************************************************************************
 * @file    PauseScreen.cpp
 * @brief   Track pause action screen implementation (Resume, Save, Discard).
 ******************************************************************************
 */

#include "gui/screens/PauseScreen.hpp"
#include "gui/screens/ScreenManager.hpp"
#include "gui/theme/Theme.hpp"

#include <cstdio>

PauseScreen::PauseScreen(Model& model)
    : Screen(model)
{
}

void PauseScreen::build()
{
    mButtons = std::make_unique<SDK::LVGL::Buttons>(mRoot);
    mButtons->set(SDK::LVGL::Buttons::WHITE,   // L1: Up
                  SDK::LVGL::Buttons::WHITE,   // L2: Down
                  SDK::LVGL::Buttons::GREEN,   // R1: Select
                  SDK::LVGL::Buttons::NONE);   // R2: None

    // Title
    mTitleLabel = Theme::label(mRoot, Theme::Font::SemiBold20, "PAUSED", 0, 16, 240,
                               LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::YELLOW_DARK);

    // Stats row
    mStatsLabel = Theme::label(mRoot, Theme::Font::Regular14, "--:--", 0, 40, 240,
                               LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::GRAY);

    static const char* kActionNames[3] = {
        "Resume",
        "Save Workout",
        "Discard"
    };

    const int16_t yPositions[3] = { 65, 107, 149 };

    for (int i = 0; i < 3; ++i) {
        mCards[i] = Theme::container(mRoot, 25, yPositions[i], 190, 36);
        lv_obj_set_style_radius(mCards[i], 8, 0);

        mLabels[i] = Theme::label(mCards[i], Theme::Font::SemiBold20, kActionNames[i], 0, 6, 190,
                                  LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::WHITE);
    }

    mNavLabel = Theme::label(mRoot, Theme::Font::Regular14, "L1/L2: Choose • R1: Select", 0, 198, 240,
                             LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::GRAY);

    updateHighlight();
}

void PauseScreen::onShow()
{
    const Track::Data& data = mModel.getTrackData();
    char buf[48];
    unsigned m = static_cast<unsigned>(data.totalTime / 60);
    unsigned s = static_cast<unsigned>(data.totalTime % 60);

    if (data.hr > 20.0f) {
        std::snprintf(buf, sizeof(buf), "%02u:%02u • %u bpm • %u cyc",
                      m, s, static_cast<unsigned>(data.hr + 0.5f), data.completedCycles);
    } else {
        std::snprintf(buf, sizeof(buf), "%02u:%02u • %u cycles", m, s, data.completedCycles);
    }
    if (mStatsLabel) {
        lv_label_set_text(mStatsLabel, buf);
    }

    mSelected = Action::Resume;
    updateHighlight();
}

void PauseScreen::updateHighlight()
{
    for (int i = 0; i < 3; ++i) {
        bool isSel = (i == static_cast<int>(mSelected));

        if (isSel) {
            uint32_t bgCol = SDK::GUI::Color::TEAL_DARK;
            uint32_t textCol = SDK::GUI::Color::WHITE;

            if (i == 0) { // Resume
                bgCol = SDK::GUI::Color::GREEN;
                textCol = SDK::GUI::Color::WHITE;
            } else if (i == 1) { // Save
                bgCol = SDK::GUI::Color::TEAL_DARK;
                textCol = SDK::GUI::Color::CYAN_PURE;
            } else if (i == 2) { // Discard
                bgCol = SDK::GUI::Color::CRIMSON;
                textCol = SDK::GUI::Color::WHITE;
            }

            lv_obj_set_style_bg_color(mCards[i], Theme::rgb(bgCol), 0);
            lv_obj_set_style_bg_opa(mCards[i], LV_OPA_90, 0);
            lv_obj_set_style_text_color(mLabels[i], Theme::rgb(textCol), 0);
        } else {
            lv_obj_set_style_bg_color(mCards[i], Theme::rgb(SDK::GUI::Color::GRAY_DARK), 0);
            lv_obj_set_style_bg_opa(mCards[i], LV_OPA_30, 0);
            lv_obj_set_style_text_color(mLabels[i], Theme::rgb(SDK::GUI::Color::GRAY), 0);
        }
    }
}

void PauseScreen::executeSelectedAction()
{
    switch (mSelected) {
        case Action::Resume:
            mModel.resumeTrack();
            ScreenManager::instance().goTo(ScreenId::Breath);
            break;

        case Action::Save:
            mModel.stopTrack(false);
            break;

        case Action::Discard:
            mModel.stopTrack(true);
            ScreenManager::instance().goTo(ScreenId::Main);
            break;

        default:
            break;
    }
}

void PauseScreen::onKey(uint8_t code)
{
    switch (code) {
        case SDK::GUI::Button::L1: { // Up
            int cur = static_cast<int>(mSelected);
            mSelected = static_cast<Action>((cur + static_cast<int>(Action::Count) - 1) % static_cast<int>(Action::Count));
            updateHighlight();
        } break;

        case SDK::GUI::Button::L2: { // Down
            int cur = static_cast<int>(mSelected);
            mSelected = static_cast<Action>((cur + 1) % static_cast<int>(Action::Count));
            updateHighlight();
        } break;

        case SDK::GUI::Button::R1: // Select
            executeSelectedAction();
            break;

        case SDK::GUI::Button::R2: // Quick resume
            mModel.resumeTrack();
            ScreenManager::instance().goTo(ScreenId::Breath);
            break;

        default:
            break;
    }
}

void PauseScreen::onTrackStateChanged(Track::State state)
{
    if (state == Track::State::COMPLETED) {
        ScreenManager::instance().goTo(ScreenId::Summary);
    } else if (state == Track::State::INACTIVE) {
        ScreenManager::instance().goTo(ScreenId::Main);
    }
}

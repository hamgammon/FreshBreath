/**
 ******************************************************************************
 * @file    PatternSelectScreen.cpp
 * @brief   Screen to browse and select breathwork patterns implementation.
 ******************************************************************************
 */

#include "gui/screens/PatternSelectScreen.hpp"
#include "gui/screens/ScreenManager.hpp"
#include "gui/theme/Theme.hpp"
#include "BreathPatterns.hpp"

#include <cstdio>

static const char* getBenefitDescription(size_t index)
{
    switch (index) {
        case 0: return "Focus & Acute Stress Reset";
        case 1: return "Deep Vagal Calm & Sleep";
        case 2: return "Max HRV & Heart Coherence";
        case 3: return "Anxiety Relief (1:2 Ratio)";
        case 4: return "Morning Alertness & Energy";
        case 5: return "Harmonious Mindfulness";
        default: return "";
    }
}

PatternSelectScreen::PatternSelectScreen(Model& model)
    : Screen(model)
    , mCurrentIndex(model.getSettings().selectedPatternIndex)
{
}

void PatternSelectScreen::build()
{
    mButtons = std::make_unique<SDK::LVGL::Buttons>(mRoot);
    mButtons->set(SDK::LVGL::Buttons::WHITE,   // L1: Prev
                  SDK::LVGL::Buttons::WHITE,   // L2: Next
                  SDK::LVGL::Buttons::GREEN,   // R1: Select
                  SDK::LVGL::Buttons::RED);    // R2: Cancel

    mTitleLabel = Theme::label(mRoot, Theme::Font::Regular14, "SELECT PATTERN", 0, 15, 240,
                               LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::GRAY);

    lv_obj_t* card = Theme::container(mRoot, 15, 38, 210, 150);
    lv_obj_set_style_bg_color(card, Theme::rgb(SDK::GUI::Color::TEAL_DARK), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_40, 0);
    lv_obj_set_style_radius(card, 10, 0);

    mPatternLabel = Theme::label(card, Theme::Font::SemiBold25, "", 0, 10, 210,
                                 LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::CYAN_PURE);

    mRatioLabel = Theme::label(card, Theme::Font::SemiBold20, "", 0, 42, 210,
                               LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::WHITE);

    mPhasesLabel = Theme::label(card, Theme::Font::Regular14, "", 0, 72, 210,
                                LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::AQUA);

    mBenefitLabel = Theme::label(card, Theme::Font::Italic18, "", 0, 108, 210,
                                 LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::YELLOW_DARK);

    mNavLabel = Theme::label(mRoot, Theme::Font::Regular14, "L1/L2: Change • R1: Select", 0, 198, 240,
                             LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::GRAY);

    updateView();
}

void PatternSelectScreen::onShow()
{
    mCurrentIndex = mModel.getSettings().selectedPatternIndex;
    updateView();
}

void PatternSelectScreen::onKey(uint8_t code)
{
    switch (code) {
        case SDK::GUI::Button::L2: // Next pattern
            mCurrentIndex = (mCurrentIndex + 1) % FreshBreath::PatternRegistry::kPatternCount;
            updateView();
            break;

        case SDK::GUI::Button::L1: // Previous pattern
            mCurrentIndex = (mCurrentIndex + FreshBreath::PatternRegistry::kPatternCount - 1)
                            % FreshBreath::PatternRegistry::kPatternCount;
            updateView();
            break;

        case SDK::GUI::Button::R1: { // Confirm selection
            Settings s = mModel.getSettings();
            s.selectedPatternIndex = mCurrentIndex;
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

void PatternSelectScreen::updateView()
{
    const FreshBreath::BreathPattern& p = FreshBreath::PatternRegistry::getPattern(mCurrentIndex);

    if (mPatternLabel) {
        lv_label_set_text(mPatternLabel, p.displayName);
    }

    if (mRatioLabel) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "Ratio: %s", p.ratioLabel);
        lv_label_set_text(mRatioLabel, buf);
    }

    if (mPhasesLabel) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "In %us • Hold %us • Out %us • Hold %us",
                      p.inhaleSec, p.holdInSec, p.exhaleSec, p.holdOutSec);
        lv_label_set_text(mPhasesLabel, buf);
    }

    if (mBenefitLabel) {
        lv_label_set_text(mBenefitLabel, getBenefitDescription(mCurrentIndex));
    }
}

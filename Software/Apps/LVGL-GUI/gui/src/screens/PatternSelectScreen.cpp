/**
 ******************************************************************************
 * @file    PatternSelectScreen.cpp
 * @brief   WheelMenu screen implementation for selecting breathwork pattern.
 ******************************************************************************
 */

#include "gui/screens/PatternSelectScreen.hpp"
#include "gui/screens/ScreenManager.hpp"
#include "gui/theme/Theme.hpp"
#include "gui/Assets.hpp"

#include <cstdio>

using namespace SDK::GUI;

namespace {
const char* const kPatternBenefits[FreshBreath::PatternRegistry::kPatternCount] = {
    "Focus",
    "Sleep",
    "HRV",
    "Calm",
    "Alert",
    "Steady"
};
} // namespace

PatternSelectScreen::PatternSelectScreen(Model& model)
    : Screen(model)
{
}

void PatternSelectScreen::build()
{
    populateItems();

    mMenu    = std::make_unique<Widgets::WheelMenu>(mRoot, mItems, kCount);
    mTitle   = std::make_unique<Widgets::Title>(mRoot, "PATTERN");
    mButtons = std::make_unique<Widgets::Buttons>(mRoot);
    mButtons->set(Widgets::Buttons::NONE,
                  Widgets::Buttons::NONE,
                  Widgets::Buttons::AMBER,
                  Widgets::Buttons::WHITE);
}

void PatternSelectScreen::onShow()
{
    populateItems();
    mMenu->refresh();

    uint16_t sel = mModel.getSettings().selectedPatternIndex;
    if (sel >= kCount) sel = 0;
    mMenu->select(sel);
}

void PatternSelectScreen::populateItems()
{
    using Style = Widgets::WheelMenu::Style;
    const uint8_t currentIdx = mModel.getSettings().selectedPatternIndex;

    for (uint16_t i = 0; i < kCount; ++i) {
        const auto& p = FreshBreath::PatternRegistry::getPattern(i);
        std::snprintf(mTips[i], sizeof(mTips[i]), "%s  %s", p.ratioLabel, kPatternBenefits[i]);

        mItems[i]            = {};
        mItems[i].style      = Style::Tip;
        mItems[i].text       = p.displayName;
        mItems[i].centerFont = &poppins_semibold_25;
        mItems[i].tip        = mTips[i];
        mItems[i].tipColor   = (i == currentIdx) ? Color::TEAL : Color::WHITE;
    }
}

void PatternSelectScreen::onKey(uint8_t code)
{
    switch (code) {
        case Btn::L1:
            mMenu->prev();
            break;

        case Btn::L2:
            mMenu->next();
            break;

        case Btn::R1: {
            Settings s = mModel.getSettings();
            s.selectedPatternIndex = static_cast<uint8_t>(mMenu->selected());
            mModel.saveSettings(s);
            mModel.setResultMode(Model::ResultMode::SettingSaved);
            ScreenManager::instance().goTo(ScreenId::Result);
        } break;

        case Btn::R2:
            ScreenManager::instance().goTo(ScreenId::Main);
            break;

        default:
            break;
    }
}

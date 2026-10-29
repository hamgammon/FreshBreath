/**
 ******************************************************************************
 * @file    DurationSelectScreen.cpp
 * @brief   WheelMenu screen implementation for selecting session duration.
 ******************************************************************************
 */

#include "gui/screens/DurationSelectScreen.hpp"
#include "gui/screens/ScreenManager.hpp"
#include "gui/theme/Theme.hpp"
#include "gui/Assets.hpp"

using namespace SDK::GUI;

namespace {
struct DurationOption {
    uint16_t    sec;
    const char* label;
    const char* desc;
};

constexpr DurationOption kOptions[] = {
    {   60, "1 min",  "Quick reset"       },
    {  120, "2 min",  "Short calm"        },
    {  180, "3 min",  "Mini break"        },
    {  300, "5 min",  "Recommended"       },
    {  600, "10 min", "Deep relaxation"   },
    {  900, "15 min", "Extended session"  },
    { 1200, "20 min", "Full meditation"   },
    {    0, "Open",   "Until stopped"     }
};
} // namespace

DurationSelectScreen::DurationSelectScreen(Model& model)
    : Screen(model)
{
}

void DurationSelectScreen::build()
{
    populateItems();

    mMenu    = std::make_unique<Widgets::WheelMenu>(mRoot, mItems, kOptionCount);
    mTitle   = std::make_unique<Widgets::Title>(mRoot, "TIME");
    mButtons = std::make_unique<Widgets::Buttons>(mRoot);
    mButtons->set(Widgets::Buttons::NONE,
                  Widgets::Buttons::NONE,
                  Widgets::Buttons::AMBER,
                  Widgets::Buttons::WHITE);
}

void DurationSelectScreen::onShow()
{
    populateItems();
    mMenu->refresh();

    const uint16_t curSec = mModel.getSettings().targetDurationSec;
    uint16_t sel = 3; // Default 5 min
    for (uint16_t i = 0; i < kOptionCount; ++i) {
        if (kOptions[i].sec == curSec) {
            sel = i;
            break;
        }
    }
    mMenu->select(sel);
}

void DurationSelectScreen::populateItems()
{
    using Style = Widgets::WheelMenu::Style;
    const uint16_t curSec = mModel.getSettings().targetDurationSec;

    for (uint16_t i = 0; i < kOptionCount; ++i) {
        mItems[i]            = {};
        mItems[i].style      = Style::Tip;
        mItems[i].text       = kOptions[i].label;
        mItems[i].centerFont = &poppins_semibold_25;
        mItems[i].tip        = kOptions[i].desc;
        mItems[i].tipColor   = (kOptions[i].sec == curSec) ? Color::TEAL : Color::WHITE;
    }
}

void DurationSelectScreen::onKey(uint8_t code)
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
            s.targetDurationSec = kOptions[mMenu->selected()].sec;
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

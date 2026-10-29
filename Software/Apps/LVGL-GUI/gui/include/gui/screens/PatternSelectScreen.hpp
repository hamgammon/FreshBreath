/**
 ******************************************************************************
 * @file    PatternSelectScreen.hpp
 * @brief   WheelMenu screen to select breathwork protocol.
 ******************************************************************************
 */

#ifndef PATTERN_SELECT_SCREEN_HPP
#define PATTERN_SELECT_SCREEN_HPP

#include <memory>
#include "gui/screens/Screen.hpp"
#include "gui/widgets/Widgets.hpp"
#include "BreathPatterns.hpp"

class PatternSelectScreen : public Screen {
public:
    explicit PatternSelectScreen(Model& model);
    ~PatternSelectScreen() override = default;

    void onShow() override;
    void onKey(uint8_t code) override;

protected:
    void build() override;

private:
    static constexpr uint16_t kCount = FreshBreath::PatternRegistry::kPatternCount;

    void populateItems();

    Widgets::WheelMenu::Item mItems[kCount]{};
    char mTips[kCount][32]{};

    std::unique_ptr<Widgets::WheelMenu> mMenu;
    std::unique_ptr<Widgets::Title>     mTitle;
    std::unique_ptr<Widgets::Buttons>   mButtons;
};

#endif // PATTERN_SELECT_SCREEN_HPP

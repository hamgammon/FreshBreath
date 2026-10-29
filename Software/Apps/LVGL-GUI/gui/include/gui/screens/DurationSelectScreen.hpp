/**
 ******************************************************************************
 * @file    DurationSelectScreen.hpp
 * @brief   WheelMenu screen to select breathwork session duration.
 ******************************************************************************
 */

#ifndef DURATION_SELECT_SCREEN_HPP
#define DURATION_SELECT_SCREEN_HPP

#include <memory>
#include "gui/screens/Screen.hpp"
#include "gui/widgets/Widgets.hpp"

class DurationSelectScreen : public Screen {
public:
    explicit DurationSelectScreen(Model& model);
    ~DurationSelectScreen() override = default;

    void onShow() override;
    void onKey(uint8_t code) override;

protected:
    void build() override;

private:
    static constexpr uint16_t kOptionCount = 8;

    void populateItems();

    Widgets::WheelMenu::Item mItems[kOptionCount]{};

    std::unique_ptr<Widgets::WheelMenu> mMenu;
    std::unique_ptr<Widgets::Title>     mTitle;
    std::unique_ptr<Widgets::Buttons>   mButtons;
};

#endif // DURATION_SELECT_SCREEN_HPP

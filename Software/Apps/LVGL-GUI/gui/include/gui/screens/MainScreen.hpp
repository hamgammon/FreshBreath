/**
 ******************************************************************************
 * @file    MainScreen.hpp
 * @brief   Main WheelMenu screen for FreshBreath (modeled after PillSync/Run).
 ******************************************************************************
 */

#ifndef MAIN_SCREEN_HPP
#define MAIN_SCREEN_HPP

#include <memory>
#include "gui/screens/Screen.hpp"
#include "gui/widgets/Widgets.hpp"

class MainScreen : public Screen {
public:
    explicit MainScreen(Model& model);
    ~MainScreen() override = default;

    void onShow() override;
    void onKey(uint8_t code) override;

    void onSettingsUpdated(const Settings& settings) override;
    void onSummaryUpdated(const ActivitySummary& summary) override;
    void onTrackStateChanged(Track::State state) override;

protected:
    void build() override;

private:
    static constexpr uint16_t kItemCount = 5;

    enum ItemIdx : uint16_t {
        ITEM_START = 0,
        ITEM_PATTERN,
        ITEM_TIME,
        ITEM_HAPTICS,
        ITEM_SUMMARY
    };

    void populateItems();

    Widgets::WheelMenu::Item mItems[kItemCount]{};
    char mStartTip[32]{};
    char mPatternTip[32]{};
    char mTimeTip[24]{};
    char mSummaryTip[32]{};

    std::unique_ptr<Widgets::WheelMenu> mMenu;
    std::unique_ptr<Widgets::Title>     mTitle;
    std::unique_ptr<Widgets::Buttons>   mButtons;
};

#endif // MAIN_SCREEN_HPP

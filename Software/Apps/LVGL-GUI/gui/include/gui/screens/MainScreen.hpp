/**
 ******************************************************************************
 * @file    MainScreen.hpp
 * @brief   FreshBreath main home screen: displays current pattern and duration.
 ******************************************************************************
 */

#ifndef MAIN_SCREEN_HPP
#define MAIN_SCREEN_HPP

#include "gui/screens/Screen.hpp"
#include <memory>

class MainScreen : public Screen {
public:
    explicit MainScreen(Model& model);
    ~MainScreen() override = default;

    void onShow() override;
    void onKey(uint8_t code) override;

    void onTimeUpdated(const std::tm& time) override;
    void onBatteryUpdated(uint8_t level) override;
    void onSettingsUpdated(const Settings& settings) override;

protected:
    void build() override;

private:
    void updateCards();
    void updateTimeLabel();

    std::unique_ptr<SDK::LVGL::Buttons> mButtons;

    lv_obj_t* mTitleLabel     = nullptr;
    lv_obj_t* mTimeLabel      = nullptr;
    lv_obj_t* mPatternCard    = nullptr;
    lv_obj_t* mPatternName    = nullptr;
    lv_obj_t* mPatternRatio   = nullptr;
    lv_obj_t* mDurationLabel  = nullptr;
    lv_obj_t* mHintLabel      = nullptr;
};

#endif // MAIN_SCREEN_HPP

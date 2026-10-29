/**
 ******************************************************************************
 * @file    CountdownScreen.hpp
 * @brief   10-second preparation countdown screen before breathing begins.
 ******************************************************************************
 */

#ifndef COUNTDOWN_SCREEN_HPP
#define COUNTDOWN_SCREEN_HPP

#include "gui/screens/Screen.hpp"
#include "SDK/GUI/LVGL/Buttons.hpp"

#include <memory>
#include <cstdint>

class CountdownScreen : public Screen {
public:
    explicit CountdownScreen(Model& model);
    ~CountdownScreen() override;

    void build() override;
    void onShow() override;
    void onHide() override;
    void onKey(uint8_t code) override;

private:
    void updateCountdown();
    void startBreathing();
    static void onTimerTick(lv_timer_t* timer);

    lv_timer_t* mTimer = nullptr;
    int8_t mSecondsLeft = 10;

    lv_obj_t* mTitleLabel = nullptr;
    lv_obj_t* mPatternLabel = nullptr;
    lv_obj_t* mCountLabel = nullptr;
    lv_obj_t* mHintLabel = nullptr;
    lv_obj_t* mCountdownArc = nullptr;
    lv_obj_t* mNavLabel = nullptr;

    std::unique_ptr<SDK::LVGL::Buttons> mButtons;
};

#endif // COUNTDOWN_SCREEN_HPP

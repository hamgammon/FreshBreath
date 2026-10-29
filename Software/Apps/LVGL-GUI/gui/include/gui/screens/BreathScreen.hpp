/**
 ******************************************************************************
 * @file    BreathScreen.hpp
 * @brief   Active breathing session screen with visual pacing and biometric stats.
 ******************************************************************************
 */

#ifndef BREATH_SCREEN_HPP
#define BREATH_SCREEN_HPP

#include "gui/screens/Screen.hpp"
#include <memory>

class BreathScreen : public Screen {
public:
    explicit BreathScreen(Model& model);
    ~BreathScreen() override = default;

    void onShow() override;
    void onHide() override;
    void onKey(uint8_t code) override;

    void onTrackDataUpdated(const Track::Data& data) override;
    void onTrackStateChanged(Track::State state) override;

protected:
    void build() override;

private:
    void updateArc(FreshBreath::BreathPhase phase, uint16_t elapsed, uint16_t duration);
    void updateLabels(const Track::Data& data);

    std::unique_ptr<SDK::LVGL::Buttons> mButtons;

    lv_obj_t* mBreathArcBg      = nullptr;
    lv_obj_t* mBreathArc        = nullptr;
    lv_obj_t* mPhaseLabel       = nullptr;
    lv_obj_t* mCountdownLabel   = nullptr;

    lv_obj_t* mHrIcon           = nullptr;
    lv_obj_t* mHrLabel          = nullptr;
    lv_obj_t* mCycleLabel       = nullptr;
    lv_obj_t* mTimerLabel       = nullptr;
    lv_obj_t* mStatusLabel      = nullptr;

    bool mIsPaused = false;
};

#endif // BREATH_SCREEN_HPP

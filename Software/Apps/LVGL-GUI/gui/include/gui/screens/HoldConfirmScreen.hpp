/**
 ******************************************************************************
 * @file    HoldConfirmScreen.hpp
 * @brief   Hold-to-confirm screen (3 -> 2 -> 1) for Save & End / Discard,
 *          modeled after RunLVGL's TrackHoldConfirmScreen.
 ******************************************************************************
 */

#ifndef HOLD_CONFIRM_SCREEN_HPP
#define HOLD_CONFIRM_SCREEN_HPP

#include <memory>
#include "gui/screens/Screen.hpp"
#include "gui/widgets/Widgets.hpp"

class HoldConfirmScreen : public Screen {
public:
    explicit HoldConfirmScreen(Model& model);
    ~HoldConfirmScreen() override;

    void onShow() override;
    void onHide() override;
    void onKey(uint8_t code) override;

protected:
    void build() override;

private:
    static constexpr uint32_t kHoldMs = 1500;

    static void animExecCb(void* var, int32_t value);
    static void animReadyCb(lv_anim_t* a);
    void setCountdown(uint32_t number);
    void cancel();

    Model::HoldConfirmMode mMode  = Model::HoldConfirmMode::Finish;
    bool                   mFired = false;
    uint32_t               mShown = 3;

    lv_obj_t* mNumber = nullptr;
    std::unique_ptr<Widgets::TimerRing> mRing;
    std::unique_ptr<Widgets::Buttons>   mButtons;
};

#endif // HOLD_CONFIRM_SCREEN_HPP

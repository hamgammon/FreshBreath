/**
 ******************************************************************************
 * @file    CountdownScreen.hpp
 * @brief   5-second preparation countdown screen (modeled after RunLVGL).
 ******************************************************************************
 */

#ifndef COUNTDOWN_SCREEN_HPP
#define COUNTDOWN_SCREEN_HPP

#include <memory>
#include "gui/screens/Screen.hpp"
#include "gui/widgets/Widgets.hpp"

class CountdownScreen : public Screen {
public:
    explicit CountdownScreen(Model& model);
    ~CountdownScreen() override;

    void onShow() override;
    void onHide() override;
    void onKey(uint8_t code) override;

protected:
    void build() override;

private:
    static constexpr uint32_t kTimeoutMs  = 5000;
    static constexpr int32_t  kCountTicks = 5;

    static void animExecCb(void* var, int32_t value);
    static void animReadyCb(lv_anim_t* a);
    void startBreathing();

    std::unique_ptr<Widgets::TimerRing> mRing;
    std::unique_ptr<Widgets::Buttons>   mButtons;

    lv_obj_t* mPattern = nullptr;
    lv_obj_t* mRatio   = nullptr;
    lv_obj_t* mTarget  = nullptr;
    lv_obj_t* mCount   = nullptr;

    int32_t mLastShownCount = -1;
    bool    mStarted        = false;
};

#endif // COUNTDOWN_SCREEN_HPP

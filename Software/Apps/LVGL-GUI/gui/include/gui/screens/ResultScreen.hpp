/**
 ******************************************************************************
 * @file    ResultScreen.hpp
 * @brief   Result feedback screen ("Saved" / "Discarded" with TimerRing),
 *          modeled after PillSync's SavedScreen and RunLVGL's TrackResultScreen.
 ******************************************************************************
 */

#ifndef RESULT_SCREEN_HPP
#define RESULT_SCREEN_HPP

#include <memory>
#include "gui/screens/Screen.hpp"
#include "gui/widgets/Widgets.hpp"

class ResultScreen : public Screen {
public:
    explicit ResultScreen(Model& model);
    ~ResultScreen() override;

    void onShow() override;
    void onHide() override;
    void onKey(uint8_t code) override;

protected:
    void build() override;

private:
    static void animExecCb(void* var, int32_t value);
    static void animReadyCb(lv_anim_t* a);
    void finish();

    std::unique_ptr<Widgets::TimerRing> mRing;
    lv_obj_t* mIcon   = nullptr;
    lv_obj_t* mStatus = nullptr;
    lv_obj_t* mDesc   = nullptr;

    uint32_t  mTimeoutMs = 1500;
    bool      mFinished  = false;
};

#endif // RESULT_SCREEN_HPP

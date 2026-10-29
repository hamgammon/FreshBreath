/**
 ******************************************************************************
 * @file    SummaryScreen.hpp
 * @brief   Completed breathwork session summary screen.
 ******************************************************************************
 */

#ifndef SUMMARY_SCREEN_HPP
#define SUMMARY_SCREEN_HPP

#include "gui/screens/Screen.hpp"
#include <memory>

class SummaryScreen : public Screen {
public:
    explicit SummaryScreen(Model& model);
    ~SummaryScreen() override = default;

    void onShow() override;
    void onKey(uint8_t code) override;

protected:
    void build() override;

private:
    void updateView();

    std::unique_ptr<SDK::LVGL::Buttons> mButtons;

    lv_obj_t* mHeaderLabel   = nullptr;
    lv_obj_t* mTitleLabel    = nullptr;
    lv_obj_t* mDurationLabel = nullptr;
    lv_obj_t* mCyclesLabel   = nullptr;
    lv_obj_t* mHrLabel       = nullptr;
    lv_obj_t* mCalLabel      = nullptr;
    lv_obj_t* mDonePrompt    = nullptr;
};

#endif // SUMMARY_SCREEN_HPP

/**
 ******************************************************************************
 * @file    PatternSelectScreen.hpp
 * @brief   Screen to browse and select breathwork patterns.
 ******************************************************************************
 */

#ifndef PATTERN_SELECT_SCREEN_HPP
#define PATTERN_SELECT_SCREEN_HPP

#include "gui/screens/Screen.hpp"
#include <memory>

class PatternSelectScreen : public Screen {
public:
    explicit PatternSelectScreen(Model& model);
    ~PatternSelectScreen() override = default;

    void onShow() override;
    void onKey(uint8_t code) override;

protected:
    void build() override;

private:
    void updateView();

    std::unique_ptr<SDK::LVGL::Buttons> mButtons;

    uint8_t mCurrentIndex = 0;

    lv_obj_t* mTitleLabel     = nullptr;
    lv_obj_t* mPatternLabel   = nullptr;
    lv_obj_t* mRatioLabel     = nullptr;
    lv_obj_t* mPhasesLabel    = nullptr;
    lv_obj_t* mBenefitLabel   = nullptr;
    lv_obj_t* mNavLabel       = nullptr;
};

#endif // PATTERN_SELECT_SCREEN_HPP

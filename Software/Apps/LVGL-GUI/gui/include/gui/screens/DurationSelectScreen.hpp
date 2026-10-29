/**
 ******************************************************************************
 * @file    DurationSelectScreen.hpp
 * @brief   Screen to choose session target duration.
 ******************************************************************************
 */

#ifndef DURATION_SELECT_SCREEN_HPP
#define DURATION_SELECT_SCREEN_HPP

#include "gui/screens/Screen.hpp"
#include <memory>

class DurationSelectScreen : public Screen {
public:
    explicit DurationSelectScreen(Model& model);
    ~DurationSelectScreen() override = default;

    void onShow() override;
    void onKey(uint8_t code) override;

protected:
    void build() override;

private:
    void updateView();

    static constexpr size_t kOptionCount = 8;
    static const uint16_t kDurations[kOptionCount];

    std::unique_ptr<SDK::LVGL::Buttons> mButtons;

    size_t mCurrentIndex = 2; // Default: 5 min

    lv_obj_t* mTitleLabel     = nullptr;
    lv_obj_t* mDurationLabel  = nullptr;
    lv_obj_t* mSubLabel       = nullptr;
    lv_obj_t* mNavLabel       = nullptr;
};

#endif // DURATION_SELECT_SCREEN_HPP

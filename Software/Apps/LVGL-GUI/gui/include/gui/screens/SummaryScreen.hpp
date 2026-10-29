/**
 ******************************************************************************
 * @file    SummaryScreen.hpp
 * @brief   Post-session workout summary screen with multi-face ScrollIndicator
 *          (modeled after RunLVGL's TrackSummaryScreen).
 ******************************************************************************
 */

#ifndef SUMMARY_SCREEN_HPP
#define SUMMARY_SCREEN_HPP

#include <memory>
#include "gui/screens/Screen.hpp"
#include "gui/widgets/Widgets.hpp"

class SummaryScreen : public Screen {
public:
    explicit SummaryScreen(Model& model);
    ~SummaryScreen() override = default;

    void onShow() override;
    void onKey(uint8_t code) override;
    void onSummaryUpdated(const ActivitySummary& summary) override;

protected:
    void build() override;

private:
    enum Face : uint8_t {
        FACE_OVERVIEW = 0,
        FACE_HEARTRATE,
        FACE_COUNT
    };

    void showFace(uint8_t face);
    void populate();

    std::unique_ptr<Widgets::ScrollIndicator> mScroll;
    std::unique_ptr<Widgets::Title>           mTitle;
    std::unique_ptr<Widgets::Buttons>         mButtons;

    lv_obj_t* mFaces[FACE_COUNT]{};

    // Face 0: Overview
    lv_obj_t* mPatternLabel  = nullptr;
    lv_obj_t* mDurationValue = nullptr;
    lv_obj_t* mCyclesValue   = nullptr;
    lv_obj_t* mHrOverviewVal = nullptr;

    // Face 1: Heart Rate & Calories
    lv_obj_t* mAvgHrValue    = nullptr;
    lv_obj_t* mMinMaxHrValue = nullptr;
    lv_obj_t* mCaloriesValue = nullptr;

    uint8_t   mCurrentFace   = FACE_OVERVIEW;
};

#endif // SUMMARY_SCREEN_HPP

/**
 ******************************************************************************
 * @file    SummaryScreen.cpp
 * @brief   Post-session workout summary screen implementation.
 ******************************************************************************
 */

#include "gui/screens/SummaryScreen.hpp"
#include "gui/screens/ScreenManager.hpp"
#include "gui/theme/Theme.hpp"

#include <cstdio>

using namespace SDK::GUI;

SummaryScreen::SummaryScreen(Model& model)
    : Screen(model)
{
}

void SummaryScreen::build()
{
    using F = Theme::Font;

    for (uint8_t i = 0; i < FACE_COUNT; ++i) {
        mFaces[i] = Theme::container(mRoot, 0, 0, 240, 240);
        Theme::setHidden(mFaces[i], true);
    }

    // --- Face 0: Overview ---
    {
        lv_obj_t* f = mFaces[FACE_OVERVIEW];
        mPatternLabel  = Theme::label(f, F::SemiBold20, "Box 4-4-4-4", 20, 48, 200, LV_TEXT_ALIGN_CENTER, Color::TEAL);
        mDurationValue = Theme::label(f, F::SemiBold35, "00:00", 20, 76, 200, LV_TEXT_ALIGN_CENTER, Color::WHITE);
        Theme::label(f, F::Italic18, "Duration", 20, 114, 200, LV_TEXT_ALIGN_CENTER, Color::GRAY);

        Theme::hline(f, 30, 140, 180, Color::GRAY_DARK);

        mCyclesValue   = Theme::label(f, F::SemiBold25, "0", 18, 148, 100, LV_TEXT_ALIGN_CENTER, Color::AQUA);
        Theme::label(f, F::Italic18, "Cycles", 18, 178, 100, LV_TEXT_ALIGN_CENTER, Color::GRAY);

        mHrOverviewVal = Theme::label(f, F::SemiBold25, "--", 122, 148, 100, LV_TEXT_ALIGN_CENTER, Color::WHITE);
        Theme::label(f, F::Italic18, "Avg HR", 122, 178, 100, LV_TEXT_ALIGN_CENTER, Color::GRAY);
    }

    // --- Face 1: Heart Rate & Calories ---
    {
        lv_obj_t* f = mFaces[FACE_HEARTRATE];
        mAvgHrValue = Theme::label(f, F::SemiBold35, "--", 20, 50, 200, LV_TEXT_ALIGN_CENTER, Color::WHITE);
        Theme::label(f, F::Italic18, "Avg. HR (bpm)", 20, 88, 200, LV_TEXT_ALIGN_CENTER, Color::GRAY);

        Theme::hline(f, 30, 116, 180, Color::GRAY_DARK);

        mMinMaxHrValue = Theme::label(f, F::SemiBold25, "--/--", 14, 128, 108, LV_TEXT_ALIGN_CENTER, Color::WHITE);
        Theme::label(f, F::Italic18, "Min / Max", 14, 158, 108, LV_TEXT_ALIGN_CENTER, Color::GRAY);

        mCaloriesValue = Theme::label(f, F::SemiBold25, "0.0", 122, 128, 100, LV_TEXT_ALIGN_CENTER, Color::WHITE);
        Theme::label(f, F::Italic18, "kcal", 122, 158, 100, LV_TEXT_ALIGN_CENTER, Color::GRAY);
    }

    mScroll  = std::make_unique<Widgets::ScrollIndicator>(mRoot, Widgets::ScrollIndicator::kSmall);
    mScroll->setCount(FACE_COUNT);

    mTitle   = std::make_unique<Widgets::Title>(mRoot, "SUMMARY");
    mButtons = std::make_unique<Widgets::Buttons>(mRoot);
    mButtons->set(Widgets::Buttons::NONE,
                  Widgets::Buttons::NONE,
                  Widgets::Buttons::AMBER,
                  Widgets::Buttons::WHITE);
}

void SummaryScreen::onShow()
{
    populate();
    showFace(FACE_OVERVIEW);
}

void SummaryScreen::onSummaryUpdated(const ActivitySummary& summary)
{
    (void)summary;
    populate();
}

void SummaryScreen::showFace(uint8_t face)
{
    if (face >= FACE_COUNT) face = 0;
    mCurrentFace = face;

    for (uint8_t i = 0; i < FACE_COUNT; ++i) {
        Theme::setHidden(mFaces[i], i != mCurrentFace);
    }

    mScroll->setActive(mCurrentFace);
    mTitle->setText(mCurrentFace == FACE_OVERVIEW ? "SUMMARY" : "HEART RATE");
}

void SummaryScreen::populate()
{
    const auto& sum = mModel.getSummary();
    char buf[32];

    if (sum.patternName[0] != '\0') {
        lv_label_set_text(mPatternLabel, sum.patternName);
    } else {
        lv_label_set_text(mPatternLabel, "Breathwork");
    }

    const unsigned mins = static_cast<unsigned>(sum.duration / 60);
    const unsigned secs = static_cast<unsigned>(sum.duration % 60);
    std::snprintf(buf, sizeof(buf), "%02u:%02u", mins, secs);
    lv_label_set_text(mDurationValue, buf);

    std::snprintf(buf, sizeof(buf), "%u", static_cast<unsigned>(sum.cycles));
    lv_label_set_text(mCyclesValue, buf);

    if (sum.hrAvg > 20.0f) {
        std::snprintf(buf, sizeof(buf), "%u", static_cast<unsigned>(sum.hrAvg + 0.5f));
    } else {
        std::snprintf(buf, sizeof(buf), "--");
    }
    lv_label_set_text(mHrOverviewVal, buf);
    lv_label_set_text(mAvgHrValue, buf);

    if (sum.hrMin > 20.0f && sum.hrMax > 20.0f) {
        std::snprintf(buf, sizeof(buf), "%u/%u",
                      static_cast<unsigned>(sum.hrMin + 0.5f),
                      static_cast<unsigned>(sum.hrMax + 0.5f));
    } else {
        std::snprintf(buf, sizeof(buf), "--/--");
    }
    lv_label_set_text(mMinMaxHrValue, buf);

    std::snprintf(buf, sizeof(buf), "%.1f", static_cast<double>(sum.calories));
    lv_label_set_text(mCaloriesValue, buf);
}

void SummaryScreen::onKey(uint8_t code)
{
    switch (code) {
        case Btn::L1:
            showFace((mCurrentFace + FACE_COUNT - 1) % FACE_COUNT);
            break;

        case Btn::L2:
            showFace((mCurrentFace + 1) % FACE_COUNT);
            break;

        case Btn::R1:
        case Btn::R2: // Back -> Return to MainScreen
            ScreenManager::instance().goTo(ScreenId::Main);
            break;

        default:
            break;
    }
}

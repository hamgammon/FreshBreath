/**
 ******************************************************************************
 * @file    BreathScreen.cpp
 * @brief   Active breathing session screen implementation.
 ******************************************************************************
 */

#include "gui/screens/BreathScreen.hpp"
#include "gui/screens/ScreenManager.hpp"
#include "gui/theme/Theme.hpp"
#include "gui/Assets.hpp"
#include "BreathPatterns.hpp"

#include <cstdio>
#include <algorithm>

BreathScreen::BreathScreen(Model& model)
    : Screen(model)
{
}

void BreathScreen::build()
{
    mButtons = std::make_unique<SDK::LVGL::Buttons>(mRoot);
    mButtons->set(SDK::LVGL::Buttons::NONE,    // L1: None
                  SDK::LVGL::Buttons::NONE,    // L2: None
                  SDK::LVGL::Buttons::AMBER,   // R1: Start/Stop -> Pause
                  SDK::LVGL::Buttons::WHITE);  // R2: Back -> Pause

    // Background track ring (subtle dark teal)
    mBreathArcBg = Theme::arc(mRoot, 120, 120, 95, 8, 0, 360, SDK::GUI::Color::TEAL_DARK, false);

    // Active animated breathing arc
    mBreathArc = Theme::arc(mRoot, 120, 120, 95, 10, 0, 0, SDK::GUI::Color::CYAN_PURE, true);

    // Top centered metric: Heart Rate
    mHrIcon = Theme::imageTinted(mRoot, &img_heart_30x30, 88, 36, SDK::GUI::Color::RED);
    mHrLabel = Theme::label(mRoot, Theme::Font::SemiBold20, "--", 122, 40, 48,
                            LV_TEXT_ALIGN_LEFT, SDK::GUI::Color::WHITE);

    // Center breathing guide: Phase Name & Countdown
    mPhaseLabel = Theme::label(mRoot, Theme::Font::SemiBold35, "INHALE", 0, 74, 240,
                               LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::CYAN_PURE);

    mCountdownLabel = Theme::label(mRoot, Theme::Font::SemiBold40, "4", 0, 114, 240,
                                   LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::WHITE);

    // Bottom centered metrics: Cycle & Elapsed / Target time
    mCycleLabel = Theme::label(mRoot, Theme::Font::Regular16, "Cycle 1", 0, 160, 240,
                               LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::AQUA);

    mTimerLabel = Theme::label(mRoot, Theme::Font::Regular16, "00:00", 0, 180, 240,
                               LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::GRAY);

    // Paused overlay banner (initially hidden)
    mStatusLabel = Theme::label(mRoot, Theme::Font::SemiBold20, "PAUSED", 0, 204, 240,
                                LV_TEXT_ALIGN_CENTER, SDK::GUI::Color::YELLOW);
    Theme::setHidden(mStatusLabel, true);
}

void BreathScreen::onShow()
{
    mIsPaused = (mModel.getTrackState() == Track::State::PAUSED);
    Theme::setHidden(mStatusLabel, !mIsPaused);
    updateLabels(mModel.getTrackData());
}

void BreathScreen::onHide()
{
}

void BreathScreen::onKey(uint8_t code)
{
    switch (code) {
        case Btn::R1: // Start/Stop button -> Pause & show action options
        case Btn::R2: // Back button -> Pause & show action options
            mModel.pauseTrack();
            mModel.setPauseCursor(0);
            ScreenManager::instance().goTo(ScreenId::Pause);
            break;

        default:
            break;
    }
}

void BreathScreen::onTrackDataUpdated(const Track::Data& data)
{
    updateLabels(data);
    updateArc(data.phase, data.phaseElapsedSec, data.phaseDurationSec);
}

void BreathScreen::onTrackStateChanged(Track::State state)
{
    if (state == Track::State::COMPLETED) {
        mModel.setMainCursor(0);
        mModel.setResultMode(Model::ResultMode::WorkoutSaved);
        ScreenManager::instance().goTo(ScreenId::Result);
    } else if (state == Track::State::PAUSED) {
        // Target duration reached -> open Pause menu with "Save & End" selected
        mModel.setPauseCursor(1);
        ScreenManager::instance().goTo(ScreenId::Pause);
    } else if (state == Track::State::INACTIVE) {
        ScreenManager::instance().goTo(ScreenId::Main);
    }
}

void BreathScreen::updateArc(FreshBreath::BreathPhase phase, uint16_t elapsed, uint16_t duration)
{
    if (!mBreathArc || duration == 0) return;

    int32_t progress = (static_cast<int32_t>(elapsed) * 360) / duration;
    progress = std::max<int32_t>(0, std::min<int32_t>(360, progress));

    switch (phase) {
        case FreshBreath::BreathPhase::INHALE:
            Theme::setArcColor(mBreathArc, SDK::GUI::Color::CYAN_PURE);
            Theme::setArc(mBreathArc, 0, std::max<int32_t>(5, progress));
            break;

        case FreshBreath::BreathPhase::HOLD_IN:
            Theme::setArcColor(mBreathArc, SDK::GUI::Color::YELLOW);
            Theme::setArc(mBreathArc, 0, 360);
            break;

        case FreshBreath::BreathPhase::EXHALE: {
            int32_t rem = 360 - progress;
            Theme::setArcColor(mBreathArc, SDK::GUI::Color::SKY);
            Theme::setArc(mBreathArc, 0, std::max<int32_t>(5, rem));
        } break;

        case FreshBreath::BreathPhase::HOLD_OUT:
            Theme::setArcColor(mBreathArc, SDK::GUI::Color::GRAY);
            Theme::setArc(mBreathArc, 0, 5);
            break;
    }
}

void BreathScreen::updateLabels(const Track::Data& data)
{
    // Phase label
    if (mPhaseLabel) {
        lv_label_set_text(mPhaseLabel, FreshBreath::PatternRegistry::phaseName(data.phase));
        switch (data.phase) {
            case FreshBreath::BreathPhase::INHALE:
                lv_obj_set_style_text_color(mPhaseLabel, Theme::rgb(SDK::GUI::Color::CYAN_PURE), 0);
                break;
            case FreshBreath::BreathPhase::HOLD_IN:
                lv_obj_set_style_text_color(mPhaseLabel, Theme::rgb(SDK::GUI::Color::YELLOW), 0);
                break;
            case FreshBreath::BreathPhase::EXHALE:
                lv_obj_set_style_text_color(mPhaseLabel, Theme::rgb(SDK::GUI::Color::SKY), 0);
                break;
            case FreshBreath::BreathPhase::HOLD_OUT:
                lv_obj_set_style_text_color(mPhaseLabel, Theme::rgb(SDK::GUI::Color::GRAY), 0);
                break;
        }
    }

    // Countdown seconds
    if (mCountdownLabel) {
        uint16_t rem = (data.phaseDurationSec > data.phaseElapsedSec)
                       ? (data.phaseDurationSec - data.phaseElapsedSec)
                       : 1;
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%u", rem);
        lv_label_set_text(mCountdownLabel, buf);
    }

    // Heart rate
    if (mHrLabel) {
        char buf[16];
        if (data.hr > 20.0f) {
            std::snprintf(buf, sizeof(buf), "%u", static_cast<unsigned>(data.hr + 0.5f));
        } else {
            std::snprintf(buf, sizeof(buf), "--");
        }
        lv_label_set_text(mHrLabel, buf);
    }

    // Cycle count
    if (mCycleLabel) {
        char buf[24];
        std::snprintf(buf, sizeof(buf), "Cycle %u", data.completedCycles + 1);
        lv_label_set_text(mCycleLabel, buf);
    }

    // Timer (elapsed / target)
    if (mTimerLabel) {
        char buf[32];
        unsigned elMin = static_cast<unsigned>(data.totalTime / 60);
        unsigned elSec = static_cast<unsigned>(data.totalTime % 60);

        if (data.targetTime > 0) {
            unsigned tgMin = static_cast<unsigned>(data.targetTime / 60);
            unsigned tgSec = static_cast<unsigned>(data.targetTime % 60);
            std::snprintf(buf, sizeof(buf), "%02u:%02u / %02u:%02u", elMin, elSec, tgMin, tgSec);
        } else {
            std::snprintf(buf, sizeof(buf), "%02u:%02u", elMin, elSec);
        }
        lv_label_set_text(mTimerLabel, buf);
    }
}

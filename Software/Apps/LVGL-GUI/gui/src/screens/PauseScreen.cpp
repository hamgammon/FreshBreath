/**
 ******************************************************************************
 * @file    PauseScreen.cpp
 * @brief   Paused activity menu screen implementation (modeled after RunLVGL).
 ******************************************************************************
 */

#include "gui/screens/PauseScreen.hpp"
#include "gui/screens/ScreenManager.hpp"
#include "gui/theme/Theme.hpp"

#include <cstdio>

using namespace SDK::GUI;

PauseScreen::PauseScreen(Model& model)
    : Screen(model)
{
}

void PauseScreen::build()
{
    mItems[ITEM_RESUME].text   = "Resume";
    mItems[ITEM_SAVE_END].text = "Save & End";
    mItems[ITEM_DISCARD].text  = "Discard";

    mMenu    = std::make_unique<Widgets::WheelMenu>(mRoot, mItems, kItemCount, -6);
    mInfo    = std::make_unique<Widgets::InfoCarousel>(mRoot, 40, 0);
    mPause   = std::make_unique<Widgets::PauseIndicator>(mRoot, 200);
    mButtons = std::make_unique<Widgets::Buttons>(mRoot);
    mButtons->set(Widgets::Buttons::NONE,
                  Widgets::Buttons::NONE,
                  Widgets::Buttons::AMBER,
                  Widgets::Buttons::WHITE);
}

void PauseScreen::onShow()
{
    mMenu->select(mModel.getPauseCursor());
    mPause->setTime(mModel.getTrackData().totalTime);
    mInfo->setCallback(&PauseScreen::carouselCb, this);
    mInfo->setCount(SLOT_COUNT);
}

void PauseScreen::onKey(uint8_t code)
{
    switch (code) {
        case Btn::L1:
            mMenu->prev();
            break;

        case Btn::L2:
            mMenu->next();
            break;

        case Btn::R1: {
            const uint16_t sel = mMenu->selected();
            mModel.setPauseCursor(sel);

            switch (sel) {
                case ITEM_RESUME:
                    mModel.resumeTrack();
                    ScreenManager::instance().goTo(ScreenId::Breath);
                    break;

                case ITEM_SAVE_END:
                    mModel.stopTrack(false);
                    mModel.setMainCursor(0);
                    mModel.setResultMode(Model::ResultMode::WorkoutSaved);
                    ScreenManager::instance().goTo(ScreenId::Result);
                    break;

                case ITEM_DISCARD:
                    mModel.stopTrack(true);
                    mModel.setMainCursor(0);
                    mModel.setResultMode(Model::ResultMode::WorkoutDiscarded);
                    ScreenManager::instance().goTo(ScreenId::Result);
                    break;

                default:
                    break;
            }
        } break;

        case Btn::R2: // Back -> Resume session
            mModel.resumeTrack();
            ScreenManager::instance().goTo(ScreenId::Breath);
            break;

        default:
            break;
    }
}

void PauseScreen::onTrackDataUpdated(const Track::Data& data)
{
    mPause->setTime(data.totalTime);
    mInfo->refresh();
}

void PauseScreen::carouselCb(void* user, int16_t index)
{
    static_cast<PauseScreen*>(user)->populateSlot(index);
}

void PauseScreen::populateSlot(int16_t index)
{
    const auto& d = mModel.getTrackData();
    char buf[24];

    switch (index) {
        case SLOT_CYCLES:
            mInfo->setTitle("CYCLES");
            std::snprintf(buf, sizeof(buf), "%u", static_cast<unsigned>(d.completedCycles));
            mInfo->setValue(buf);
            break;

        case SLOT_AVG_HR:
            mInfo->setTitle("AVG. HR");
            if (d.hrAvg > 20.0f) {
                std::snprintf(buf, sizeof(buf), "%u bpm", static_cast<unsigned>(d.hrAvg + 0.5f));
            } else {
                std::snprintf(buf, sizeof(buf), "---");
            }
            mInfo->setValue(buf);
            break;

        case SLOT_CALORIES:
            mInfo->setTitle("CALORIES");
            std::snprintf(buf, sizeof(buf), "%.1f kcal", static_cast<double>(d.totalCalories));
            mInfo->setValue(buf);
            break;

        default:
            break;
    }
}

/**
 ******************************************************************************
 * @file    Model.cpp
 * @brief   GUI state model implementation.
 ******************************************************************************
 */

#include "gui/model/Model.hpp"
#include "SDK/Kernel/KernelProviderGUI.hpp"
#include "SDK/Messages/MessageGuard.hpp"

#define LOG_MODULE_PRX      "Model"
#define LOG_MODULE_LEVEL    LOG_LEVEL_INFO
#include "SDK/UnaLogger/Logger.h"

Model::Model()
    : mKernel(SDK::KernelProviderGUI::GetInstance().getKernel())
{
    SDK::LVGL::Port::GetInstance().setAppLifeCycleCallback(this);
    SDK::LVGL::Port::GetInstance().setCustomMessageHandler(this);
}

Model::~Model()
{
    SDK::LVGL::Port::GetInstance().setAppLifeCycleCallback(nullptr);
    SDK::LVGL::Port::GetInstance().setCustomMessageHandler(nullptr);
}

void Model::startTrack(uint8_t patternIndex, uint16_t durationSec)
{
    SDK::send_msg<CustomMessage::TrackStartReq>(mKernel, patternIndex, durationSec);
}

void Model::pauseTrack()
{
    SDK::send_msg<CustomMessage::TrackPauseReq>(mKernel);
}

void Model::resumeTrack()
{
    SDK::send_msg<CustomMessage::TrackResumeReq>(mKernel);
}

void Model::stopTrack(bool discard)
{
    if (discard) {
        SDK::send_msg<CustomMessage::TrackDiscardReq>(mKernel);
    } else {
        SDK::send_msg<CustomMessage::TrackStopReq>(mKernel);
    }
}

void Model::saveSettings(const Settings& settings)
{
    mSettings = settings;
    SDK::send_msg<CustomMessage::SettingsSave>(mKernel, settings);
}

void Model::exitApp()
{
    SDK::LVGL::Port::GetInstance().setAppLifeCycleCallback(nullptr);
    SDK::LVGL::Port::GetInstance().setCustomMessageHandler(nullptr);
    mKernel.sys.exit();
}

void Model::handleKeyEvent(uint8_t key)
{
    (void)key;
}

bool Model::customMessageHandler(SDK::MessageBase* message)
{
    if (!message) return false;

    switch (message->getType()) {
        case CustomMessage::TRACK_STATE_UPDATE: {
            auto* msg = static_cast<CustomMessage::TrackStateUpd*>(message);
            mTrackState = msg->state;
            if (mListener) mListener->onTrackStateChanged(mTrackState);
            return true;
        }

        case CustomMessage::TRACK_DATA_UPDATE: {
            auto* msg = static_cast<CustomMessage::TrackDataUpd*>(message);
            mTrackData = msg->data;
            if (mListener) mListener->onTrackDataUpdated(mTrackData);
            return true;
        }

        case CustomMessage::SETTINGS_UPDATE: {
            auto* msg = static_cast<CustomMessage::SettingsUpd*>(message);
            mSettings = msg->settings;
            mTimeFormat12h = msg->timeFormat12h;
            if (mListener) mListener->onSettingsUpdated(mSettings);
            return true;
        }

        case CustomMessage::SUMMARY: {
            auto* msg = static_cast<CustomMessage::SummaryUpd*>(message);
            mSummary = msg->summary;
            if (mListener) mListener->onSummaryUpdated(mSummary);
            return true;
        }

        case CustomMessage::LOCAL_TIME: {
            auto* msg = static_cast<CustomMessage::Time*>(message);
            mTime = msg->localTime;
            if (mListener) mListener->onTimeUpdated(mTime);
            return true;
        }

        case CustomMessage::BATTERY: {
            auto* msg = static_cast<CustomMessage::Battery*>(message);
            mBatteryLevel = msg->level;
            if (mListener) mListener->onBatteryUpdated(mBatteryLevel);
            return true;
        }

        default:
            return false;
    }
}

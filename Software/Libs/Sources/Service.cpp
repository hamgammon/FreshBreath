/**
 ******************************************************************************
 * @file    Service.cpp
 * @brief   FreshBreath Service process implementation.
 ******************************************************************************
 */

#include "Service.hpp"

#include <cmath>
#include <cstring>
#include <algorithm>

#include "SDK/Messages/SensorLayerMessages.hpp"
#include "SDK/Messages/MessageGuard.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRateEx.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserBatteryLevel.hpp"
#include "SDK/Timer/Timer.hpp"

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_INFO
#include "SDK/UnaLogger/Logger.h"

Service::Service(SDK::Kernel& kernel)
    : mKernel(kernel)
    , mSettingsSerializer(kernel, "settings.json")
    , mActivitySummarySerializer(kernel, "Activity/summary.json")
    , mActivityWriter(kernel, "Activity")
    , mSensorHr(SDK::Sensor::Type::HEART_RATE_EX, 1000, 100)
    , mSensorBattery(SDK::Sensor::Type::BATTERY_LEVEL)
    , mCurrentPattern(FreshBreath::PatternRegistry::getPattern(0))
{
}

Service::~Service()
{
    disconnectSensors();
}

void Service::run()
{
    LOG_INFO("FreshBreath Service started\n");

    if (!mSettingsSerializer.load(mSettings)) {
        LOG_INFO("No saved settings found, using defaults\n");
    }

    if (!mActivitySummarySerializer.load(mSummary)) {
        LOG_INFO("No saved summary found\n");
    }

    if (mActivityWriter.recoverInterrupted()) {
        LOG_INFO("Recovered interrupted activity\n");
    }

    SDK::Timer guiInitTimeout(TIMER_SECONDS(8));
    guiInitTimeout.start();

    std::time_t lastTickUtc = 0;

    while (true) {
        SDK::MessageBase* msg = nullptr;
        if (mKernel.comm.getMessage(msg, 500)) {
            switch (msg->getType()) {
                case SDK::MessageType::COMMAND_APP_STOP:
                    LOG_INFO("Application stop command received\n");
                    disconnectSensors();
                    if (mTrackState == Track::State::ACTIVE || mTrackState == Track::State::PAUSED) {
                        stopTrack(false);
                    }
                    mKernel.comm.releaseMessage(msg);
                    return;

                case SDK::MessageType::COMMAND_APP_NOTIF_GUI_RUN:
                    LOG_INFO("GUI process active\n");
                    onStartGUI();
                    break;

                case SDK::MessageType::COMMAND_APP_NOTIF_GUI_STOP:
                    LOG_INFO("GUI process stopped\n");
                    onStopGUI();
                    break;

                case CustomMessage::TRACK_START: {
                    auto* req = static_cast<CustomMessage::TrackStartReq*>(msg);
                    LOG_INFO("TRACK_START req: pattern %u, duration %u\n", req->patternIndex, req->targetDuration);
                    startTrack(req->patternIndex, req->targetDuration);
                } break;

                case CustomMessage::TRACK_PAUSE:
                    LOG_INFO("TRACK_PAUSE req\n");
                    pauseTrack();
                    break;

                case CustomMessage::TRACK_RESUME:
                    LOG_INFO("TRACK_RESUME req\n");
                    resumeTrack();
                    break;

                case CustomMessage::TRACK_STOP:
                    LOG_INFO("TRACK_STOP req\n");
                    stopTrack(false);
                    break;

                case CustomMessage::TRACK_DISCARD:
                    LOG_INFO("TRACK_DISCARD req\n");
                    stopTrack(true);
                    break;

                case CustomMessage::SETTINGS_SAVE: {
                    auto* req = static_cast<CustomMessage::SettingsSave*>(msg);
                    const bool modeChanged = (mSettings.vibrateOnly != req->settings.vibrateOnly);
                    mSettings = req->settings;
                    mSettingsSerializer.save(mSettings);
                    LOG_INFO("Settings saved: pattern=%u, dur=%u, vibrateOnly=%d\n",
                             mSettings.selectedPatternIndex, mSettings.targetDurationSec, mSettings.vibrateOnly);
                    if (modeChanged) {
                        playVibroPattern(SDK::Message::RequestVibroPlay::Effect::STRONG_BUZZ_100, 1);
                        if (!mSettings.vibrateOnly) {
                            static const SDK::Message::RequestBuzzerPlay::Note kSampleBeep[] = {
                                { 150, 100, 0, 1568 }
                            };
                            playBuzzerNotes(kSampleBeep, 1);
                        }
                    }
                } break;

                case SDK::MessageType::EVENT_SENSOR_LAYER_DATA: {
                    auto* event = static_cast<SDK::Message::Sensor::EventData*>(msg);
                    SDK::Sensor::DataBatch batch(event->data, event->count, event->stride);
                    handleSensorsData(event->handle, batch);
                } break;

                default:
                    break;
            }
            mKernel.comm.releaseMessage(msg);
        }

        if (mGuiStarted) {
            std::time_t nowUtc = std::time(nullptr);
            if (nowUtc != lastTickUtc) {
                lastTickUtc = nowUtc;

                std::tm tmNow{};
#if WIN32
                localtime_s(&tmNow, &nowUtc);
#else
                localtime_r(&nowUtc, &tmNow);
#endif
                SDK::send_msg<CustomMessage::Time>(mKernel, tmNow);
                SDK::send_msg<CustomMessage::Battery>(mKernel, mBatteryLevel);

                if (mTrackState == Track::State::ACTIVE) {
                    processTrackTick();
                }
            }
        } else {
            if (guiInitTimeout.expired()) {
                LOG_INFO("GUI timeout expired, exiting service\n");
                return;
            }
        }
    }
}

void Service::onStartGUI()
{
    mGuiStarted = true;

    bool tf12 = false;
    SDK::send_msg<CustomMessage::SettingsUpd>(mKernel, mSettings, tf12);
    SDK::send_msg<CustomMessage::SummaryUpd>(mKernel, mSummary);
    SDK::send_msg<CustomMessage::TrackStateUpd>(mKernel, mTrackState);

    mSensorBattery.connect();
}

void Service::onStopGUI()
{
    mGuiStarted = false;
    if (mTrackState == Track::State::INACTIVE || mTrackState == Track::State::COMPLETED) {
        disconnectSensors();
    }
}

void Service::startTrack(uint8_t patternIndex, uint16_t targetDurationSec)
{
    mCurrentPattern = FreshBreath::PatternRegistry::getPattern(patternIndex);
    mSettings.selectedPatternIndex = patternIndex;
    mSettings.targetDurationSec = targetDurationSec;
    mSettingsSerializer.save(mSettings);

    mStartUtc = std::time(nullptr);
    mLastRecordUtc = mStartUtc;

    mTrackData = Track::Data{};
    mTrackData.patternIndex = patternIndex;
    mTrackData.targetTime = targetDurationSec;
    mTrackData.totalTime = 0;
    mTrackData.phase = FreshBreath::BreathPhase::INHALE;
    mTrackData.phaseElapsedSec = 0;
    mTrackData.phaseDurationSec = mCurrentPattern.inhaleSec;
    mTrackData.completedCycles = 0;
    mTrackData.hr = 0.0f;
    mTrackData.hrAvg = 0.0f;
    mTrackData.hrMax = 0.0f;
    mTrackData.hrMin = 999.0f;
    mTrackData.totalCalories = 0.0f;

    mHrSum = 0.0f;
    mHrCount = 0;

    mTrackState = Track::State::ACTIVE;

    connectSensors();

    ActivityWriter::AppInfo info{};
    info.timestamp = mStartUtc;
    info.appID = "432197B5C95258BF";
    info.devID = "UNA";
    mActivityWriter.start(info);

    triggerPhaseHaptic(FreshBreath::BreathPhase::INHALE);

    SDK::send_msg<CustomMessage::TrackStateUpd>(mKernel, mTrackState);
    SDK::send_msg<CustomMessage::TrackDataUpd>(mKernel, mTrackData);
}

void Service::pauseTrack()
{
    if (mTrackState == Track::State::ACTIVE) {
        mTrackState = Track::State::PAUSED;
        mActivityWriter.pause(std::time(nullptr));
        playVibroPattern(SDK::Message::RequestVibroPlay::Effect::STRONG_BUZZ_100, 2, 100);
        if (!mSettings.vibrateOnly) {
            static const SDK::Message::RequestBuzzerPlay::Note kPauseNotes[] = {
                { 120, 100, 0, 1318 },
                {  80,   0, 0, 1318 },
                { 120, 100, 0, 1047 }
            };
            playBuzzerNotes(kPauseNotes, 3);
        }
        SDK::send_msg<CustomMessage::TrackStateUpd>(mKernel, mTrackState);
    }
}

void Service::resumeTrack()
{
    if (mTrackState == Track::State::PAUSED) {
        mTrackState = Track::State::ACTIVE;
        mActivityWriter.resume(std::time(nullptr));
        triggerPhaseHaptic(mTrackData.phase);
        SDK::send_msg<CustomMessage::TrackStateUpd>(mKernel, mTrackState);
    }
}

void Service::stopTrack(bool discard)
{
    if (mTrackState == Track::State::INACTIVE) {
        return;
    }

    disconnectSensors();

    if (discard) {
        mActivityWriter.discard();
        mTrackState = Track::State::INACTIVE;
        SDK::send_msg<CustomMessage::TrackStateUpd>(mKernel, mTrackState);
        LOG_INFO("Activity discarded\n");
        return;
    }

    std::time_t endUtc = std::time(nullptr);

    ActivityWriter::TrackData track{};
    track.timestamp = endUtc;
    track.timeStart = mStartUtc;
    track.duration = mTrackData.totalTime;
    track.elapsed = mTrackData.totalTime;
    track.hrAvg = mTrackData.hrAvg;
    track.hrMax = mTrackData.hrMax;
    track.hrMin = (mTrackData.hrMin < 300.0f && mTrackData.hrMin > 0.0f) ? mTrackData.hrMin : mTrackData.hrAvg;
    track.calories = mTrackData.totalCalories;
    track.metabolicCalories = mTrackData.totalCalories * 0.85f;
    track.cycles = mTrackData.completedCycles;
    track.patternName = mCurrentPattern.displayName;
    track.workoutTitle = mCurrentPattern.workoutTitle;

    mActivityWriter.stop(track);

    // Save summary for on-device recap screen
    mSummary.utc = track.timeStart;
    mSummary.duration = track.duration;
    mSummary.hrAvg = track.hrAvg;
    mSummary.hrMax = track.hrMax;
    mSummary.hrMin = track.hrMin;
    mSummary.calories = track.calories;
    mSummary.cycles = track.cycles;
    std::snprintf(mSummary.patternName, sizeof(mSummary.patternName), "%s", mCurrentPattern.displayName);
    std::snprintf(mSummary.workoutTitle, sizeof(mSummary.workoutTitle), "%s", mCurrentPattern.workoutTitle);
    mActivitySummarySerializer.save(mSummary);

    mTrackState = Track::State::COMPLETED;
    playVibroPattern(SDK::Message::RequestVibroPlay::Effect::ALERT_750MS_100, 3, 200);
    if (!mSettings.vibrateOnly) {
        static const SDK::Message::RequestBuzzerPlay::Note kCompleteChime[] = {
            { 300, 100, 0, 1318 },
            { 120,   0, 0, 1318 },
            { 300, 100, 0, 1568 },
            { 120,   0, 0, 1568 },
            { 400, 100, 0, 2093 }
        };
        playBuzzerNotes(kCompleteChime, 5);
    }

    SDK::send_msg<CustomMessage::SummaryUpd>(mKernel, mSummary);
    SDK::send_msg<CustomMessage::TrackStateUpd>(mKernel, mTrackState);

    LOG_INFO("Saved activity [%s] duration=%llds cycles=%u hrAvg=%.1f\n",
             track.workoutTitle.c_str(), static_cast<long long>(track.duration),
             track.cycles, track.hrAvg);
}

void Service::processTrackTick()
{
    mTrackData.totalTime++;
    mTrackData.phaseElapsedSec++;

    // Check if target duration reached
    if (mTrackData.targetTime > 0 && mTrackData.totalTime >= mTrackData.targetTime) {
        stopTrack(false);
        return;
    }

    if (mTrackData.phaseElapsedSec >= mTrackData.phaseDurationSec) {
        transitionToNextPhase();
    }

    // HR statistics update
    if (mTrackData.hr > 20.0f) {
        mHrSum += mTrackData.hr;
        mHrCount++;
        mTrackData.hrAvg = mHrSum / mHrCount;
        if (mTrackData.hr > mTrackData.hrMax) mTrackData.hrMax = mTrackData.hr;
        if (mTrackData.hr < mTrackData.hrMin) mTrackData.hrMin = mTrackData.hr;

        // Breathwork/Meditation caloric expenditure (~1.25 MET)
        mTrackData.totalCalories += 0.025f;
    }

    // Write FIT record
    std::time_t nowUtc = std::time(nullptr);
    ActivityWriter::RecordData record{};
    record.timestamp = nowUtc;
    record.heartRate = mTrackData.hr;
    record.batteryLevel = mBatteryLevel;
    mActivityWriter.addRecord(record);

    SDK::send_msg<CustomMessage::TrackDataUpd>(mKernel, mTrackData);
}

void Service::transitionToNextPhase()
{
    mTrackData.phaseElapsedSec = 0;

    switch (mTrackData.phase) {
        case FreshBreath::BreathPhase::INHALE:
            if (mCurrentPattern.holdInSec > 0) {
                mTrackData.phase = FreshBreath::BreathPhase::HOLD_IN;
                mTrackData.phaseDurationSec = mCurrentPattern.holdInSec;
            } else {
                mTrackData.phase = FreshBreath::BreathPhase::EXHALE;
                mTrackData.phaseDurationSec = mCurrentPattern.exhaleSec;
            }
            break;

        case FreshBreath::BreathPhase::HOLD_IN:
            mTrackData.phase = FreshBreath::BreathPhase::EXHALE;
            mTrackData.phaseDurationSec = mCurrentPattern.exhaleSec;
            break;

        case FreshBreath::BreathPhase::EXHALE:
            if (mCurrentPattern.holdOutSec > 0) {
                mTrackData.phase = FreshBreath::BreathPhase::HOLD_OUT;
                mTrackData.phaseDurationSec = mCurrentPattern.holdOutSec;
            } else {
                mTrackData.completedCycles++;
                mTrackData.phase = FreshBreath::BreathPhase::INHALE;
                mTrackData.phaseDurationSec = mCurrentPattern.inhaleSec;
            }
            break;

        case FreshBreath::BreathPhase::HOLD_OUT:
            mTrackData.completedCycles++;
            mTrackData.phase = FreshBreath::BreathPhase::INHALE;
            mTrackData.phaseDurationSec = mCurrentPattern.inhaleSec;
            break;
    }

    triggerPhaseHaptic(mTrackData.phase);
}

void Service::triggerPhaseHaptic(FreshBreath::BreathPhase phase)
{
    switch (phase) {
        case FreshBreath::BreathPhase::INHALE:
            // Strong double 750ms pulse: prompt to inhale
            playVibroPattern(SDK::Message::RequestVibroPlay::Effect::ALERT_750MS_100, 2, 150);
            if (!mSettings.vibrateOnly) {
                static const SDK::Message::RequestBuzzerPlay::Note kInhaleNotes[] = {
                    { 180, 100, 0, 1318 },
                    {  80,   0, 0, 1318 },
                    { 220, 100, 0, 1760 }
                };
                playBuzzerNotes(kInhaleNotes, 3);
            }
            break;

        case FreshBreath::BreathPhase::HOLD_IN:
            // Strong 750ms pulse: hold breath at top
            playVibroPattern(SDK::Message::RequestVibroPlay::Effect::ALERT_750MS_100, 1);
            if (!mSettings.vibrateOnly) {
                static const SDK::Message::RequestBuzzerPlay::Note kHoldInNotes[] = {
                    { 200, 100, 0, 1568 }
                };
                playBuzzerNotes(kHoldInNotes, 1);
            }
            break;

        case FreshBreath::BreathPhase::EXHALE:
            // Long 1000ms full-power buzz: exhale
            playVibroPattern(SDK::Message::RequestVibroPlay::Effect::ALERT_1000MS_100, 1);
            if (!mSettings.vibrateOnly) {
                static const SDK::Message::RequestBuzzerPlay::Note kExhaleNotes[] = {
                    { 180, 100, 0, 1760 },
                    {  80,   0, 0, 1760 },
                    { 250, 100, 0, 1318 }
                };
                playBuzzerNotes(kExhaleNotes, 3);
            }
            break;

        case FreshBreath::BreathPhase::HOLD_OUT:
            // Strong 750ms pulse: hold breath at bottom
            playVibroPattern(SDK::Message::RequestVibroPlay::Effect::ALERT_750MS_100, 1);
            if (!mSettings.vibrateOnly) {
                static const SDK::Message::RequestBuzzerPlay::Note kHoldOutNotes[] = {
                    { 200, 100, 0, 1047 }
                };
                playBuzzerNotes(kHoldOutNotes, 1);
            }
            break;
    }
}

void Service::playVibroPattern(SDK::Message::RequestVibroPlay::Effect effect,
                               uint8_t count, uint16_t silenceMs)
{
    if (count == 0) return;

    const uint8_t maxCount = (SDK::Message::RequestVibroPlay::skMaxNotes + 1u) / 2u;
    if (count > maxCount) {
        count = maxCount;
    }

    auto* msg = mKernel.comm.allocateMessage<SDK::Message::RequestVibroPlay>();
    if (!msg) return;

    uint8_t n = 0;
    for (uint8_t i = 0; i < count; ++i) {
        msg->notes[n].effect = static_cast<uint8_t>(effect);
        msg->notes[n].pause  = 0;
        ++n;
        if (i < count - 1u) {
            msg->notes[n].effect = static_cast<uint8_t>(SDK::Message::RequestVibroPlay::Effect::NO_EFFECT);
            msg->notes[n].pause  = silenceMs;
            ++n;
        }
    }
    msg->notesCount = n;
    mKernel.comm.sendMessage(msg);
    mKernel.comm.releaseMessage(msg);
}

void Service::playBuzzerNotes(const SDK::Message::RequestBuzzerPlay::Note notes[],
                              uint8_t count)
{
    if (count == 0) return;
    if (count > SDK::Message::RequestBuzzerPlay::skMaxNotes) {
        count = SDK::Message::RequestBuzzerPlay::skMaxNotes;
    }

    auto* msg = mKernel.comm.allocateMessage<SDK::Message::RequestBuzzerPlay>();
    if (!msg) return;

    for (uint8_t i = 0; i < count; ++i) {
        msg->notes[i] = notes[i];
    }
    msg->notesCount = count;
    mKernel.comm.sendMessage(msg);
    mKernel.comm.releaseMessage(msg);
}

void Service::connectSensors()
{
    if (!mSensorsConnected) {
        mSensorHr.connect();
        mSensorBattery.connect();
        mSensorsConnected = true;
    }
}

void Service::disconnectSensors()
{
    if (mSensorsConnected) {
        mSensorHr.disconnect();
        mSensorBattery.disconnect();
        mSensorsConnected = false;
    }
}

void Service::handleSensorsData(uint16_t handle, SDK::Sensor::DataBatch& data)
{
    if (mSensorHr.matchesDriver(handle) && data.size() > 0) {
        SDK::SensorDataParser::HeartRateEx parser(data[0]);
        if (parser.isDataValid()) {
            mTrackData.hr = parser.getBpm();
        }
    } else if (mSensorBattery.matchesDriver(handle) && data.size() > 0) {
        SDK::SensorDataParser::BatteryLevel parser(data[0]);
        if (parser.isDataValid()) {
            mBatteryLevel = static_cast<uint8_t>(parser.getCharge() + 0.5f);
        }
    }
}

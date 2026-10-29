/**
 ******************************************************************************
 * @file    Service.hpp
 * @brief   FreshBreath Service process: tracks breathing session, records sensors,
 *          drives haptic/buzzer pacing, and writes FIT / JSON workout data.
 ******************************************************************************
 */

#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <ctime>
#include <memory>
#include <cstdint>

#include "SDK/Kernel/Kernel.hpp"
#include "SDK/SensorLayer/SensorConnection.hpp"
#include "SDK/SensorLayer/SensorDataBatch.hpp"
#include "SDK/Messages/CommandMessages.hpp"

#include "Settings.hpp"
#include "SettingsSerializer.hpp"
#include "ActivitySummary.hpp"
#include "ActivitySummarySerializer.hpp"
#include "ActivityWriter.hpp"
#include "Track.hpp"
#include "BreathPatterns.hpp"
#include "Commands.hpp"

class Service {
public:
    explicit Service(SDK::Kernel& kernel);
    ~Service();

    void run();

private:
    void onStartGUI();
    void onStopGUI();

    void startTrack(uint8_t patternIndex, uint16_t targetDurationSec);
    void pauseTrack();
    void resumeTrack();
    void stopTrack(bool discard = false);
    void processTrackTick();

    void connectSensors();
    void disconnectSensors();
    void handleSensorsData(uint16_t handle, SDK::Sensor::DataBatch& data);

    void playVibroPattern(SDK::Message::RequestVibroPlay::Effect effect,
                          uint8_t count = 1, uint16_t silenceMs = 150);
    void playBuzzerNotes(const SDK::Message::RequestBuzzerPlay::Note notes[],
                         uint8_t count);

    void transitionToNextPhase();
    void triggerPhaseHaptic(FreshBreath::BreathPhase phase);

    SDK::Kernel& mKernel;

    bool mGuiStarted = false;
    Settings mSettings{};
    SettingsSerializer mSettingsSerializer;

    ActivitySummary mSummary{};
    ActivitySummarySerializer mActivitySummarySerializer;
    ActivityWriter mActivityWriter;

    // Sensors
    SDK::Sensor::Connection mSensorHr;
    SDK::Sensor::Connection mSensorBattery;
    bool mSensorsConnected = false;

    // Track state
    Track::State mTrackState = Track::State::INACTIVE;
    Track::Data  mTrackData{};
    FreshBreath::BreathPattern mCurrentPattern;
    std::time_t  mStartUtc = 0;
    std::time_t  mLastRecordUtc = 0;

    // HR stats accumulation
    float    mHrSum = 0.0f;
    uint32_t mHrCount = 0;
    uint8_t  mBatteryLevel = 100;
};

#endif // SERVICE_HPP

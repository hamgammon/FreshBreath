/**
 ******************************************************************************
 * @file    ActivityWriter.hpp
 * @brief   Serializes breathwork activity data to a FIT file and JSON summary.
 ******************************************************************************
 */

#ifndef ACTIVITY_WRITER_HPP
#define ACTIVITY_WRITER_HPP

#include <cstdint>
#include <ctime>
#include <memory>
#include <string>

#include "SDK/Kernel/Kernel.hpp"
#include "SDK/Fit/FitProfile.hpp"
#include "SDK/Fit/FitWriter.hpp"
#include "SDK/Fit/RecordingMarker.hpp"

class ActivityWriter {
public:
    struct AppInfo {
        std::time_t timestamp  = 0;
        uint32_t    appVersion = 0;
        std::string devID;
        std::string appID;
    };

    struct RecordData {
        std::time_t timestamp      = 0;
        float       heartRate      = 0.0f;
        uint8_t     hrSource       = 0;
        uint8_t     hrOpticalBpm   = 0;
        uint8_t     hrExternalBpm  = 0;
        uint8_t     batteryLevel   = 0;
        uint16_t    batteryVoltage = 0;
    };

    struct LapData {
        std::time_t timestamp       = 0;
        std::time_t timeStart       = 0;
        std::time_t duration        = 0;
        std::time_t elapsed         = 0;
        float       hrAvg           = 0.0f;
        float       hrMax           = 0.0f;
        float       calories        = 0.0f;
        float       restingCalories = 0.0f;
    };

    struct TrackData {
        std::time_t timestamp          = 0;
        std::time_t timeStart          = 0;
        std::time_t duration           = 0;
        std::time_t elapsed            = 0;
        float       hrAvg              = 0.0f;
        float       hrMax              = 0.0f;
        float       hrMin              = 0.0f;
        float       calories           = 0.0f;
        float       metabolicCalories  = 0.0f;
        uint16_t    cycles             = 0;
        std::string patternName;
        std::string workoutTitle;
    };

    ActivityWriter(const SDK::Kernel& kernel, const char* pathToDir);

    void start(const AppInfo& info);
    void pause(std::time_t timestamp);
    void resume(std::time_t timestamp);
    void addRecord(const RecordData& record);
    void addLap(const LapData& lap);
    bool stop(const TrackData& track);
    void discard();

    bool recoverInterrupted();

private:
    enum Local : uint8_t {
        L_FILE_ID = 0,
        L_DEV_ID,
        L_FIELD_DESC,
        L_EVENT,
        L_RECORD,
        L_LAP,
        L_SESSION,
        L_ACTIVITY,
    };

    enum DevField : uint8_t {
        DF_BATTERY_LEVEL   = 2,
        DF_BATTERY_VOLTAGE = 3,
        DF_LAP_RESTING_CAL = 4,
        DF_HR_SOURCE       = 5,
        DF_HR_OPTICAL      = 6,
        DF_HR_EXTERNAL     = 7,
    };

    static constexpr std::time_t skFlushIntervalSec = 30;

    void defineRecordMessages();
    void writeFieldDescription(uint8_t devFieldNum, const char* name,
                               const char* units, SDK::Fit::BaseType baseType);
    void addMessageEvent(std::time_t t, SDK::Fit::EventType type);
    bool createAndOpenFile(std::time_t utc);
    bool saveSummary(const TrackData& track);

    static std::time_t tm2epoch(const struct tm* tm);
    static std::time_t epochToLocal(std::time_t utc);
    static uint32_t unixToFitTimestamp(std::time_t unixTimestamp);

    const SDK::Kernel&            mKernel;
    const char*                   mPath;
    std::unique_ptr<SDK::Interface::IFile> mFile;
    std::unique_ptr<SDK::Fit::FitWriter>   mFit;
    SDK::Fit::RecordingMarker     mMarker;
    uint16_t                      mLapCounter   = 0;
    std::time_t                   mLastFlushUtc = 0;
};

#endif // ACTIVITY_WRITER_HPP

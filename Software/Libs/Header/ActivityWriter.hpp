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
        uint8_t     batteryLevel   = 0;
        uint16_t    batteryVoltage = 0;
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
    bool stop(const TrackData& track);
    void discard();

    bool recoverInterrupted();

private:
    static constexpr uint8_t L_FILE_ID     = 0;
    static constexpr uint8_t L_DEV_ID      = 1;
    static constexpr uint8_t L_FIELD_DESC  = 2;
    static constexpr uint8_t L_EVENT       = 3;
    static constexpr uint8_t L_RECORD      = 4;
    static constexpr uint8_t L_WORKOUT     = 5;
    static constexpr uint8_t L_SESSION     = 6;
    static constexpr uint8_t L_ACTIVITY    = 7;

    static constexpr uint8_t DF_BATTERY_LEVEL  = 0;
    static constexpr uint8_t DF_WORKOUT_TITLE  = 1;
    static constexpr uint8_t DF_PATTERN_NAME   = 2;
    static constexpr uint8_t DF_CYCLES         = 3;

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

    std::time_t mLastFlushUtc = 0;
};

#endif // ACTIVITY_WRITER_HPP

/**
 ******************************************************************************
 * @file    ActivityWriter.cpp
 * @brief   Serializes breathwork activity data to a FIT file and JSON summary.
 ******************************************************************************
 */

#include "ActivityWriter.hpp"

#include "SDK/Interfaces/IFileSystem.hpp"
#include "SDK/JSON/JsonStreamWriter.hpp"

#include <cassert>
#include <cstring>

#define LOG_MODULE_PRX      "ActivityWriter"
#define LOG_MODULE_LEVEL    LOG_LEVEL_INFO
#include "SDK/UnaLogger/Logger.h"

namespace fit = SDK::Fit;

ActivityWriter::ActivityWriter(const SDK::Kernel& kernel, const char* pathToDir)
    : mKernel(kernel), mPath(pathToDir), mMarker(kernel.fs, pathToDir)
{
    assert(pathToDir != nullptr);
}

void ActivityWriter::start(const AppInfo& info)
{
    mLastFlushUtc = info.timestamp;

    if (!createAndOpenFile(info.timestamp)) {
        return;
    }

    mFit = std::make_unique<fit::FitWriter>(*mFile);
    const bool begun = mFit->begin(/*profileVersion=*/0);
    if (!begun) {
        LOG_ERROR("Failed to write FIT header\n");
    }

    // file_id
    const uint8_t productNameLen =
        static_cast<uint8_t>(std::strlen(fit::kProductName) + 1);
    mFit->defineMessage(L_FILE_ID, fit::mesgNum(fit::MesgNum::FileId),
        {fit::field::FileId::Type, fit::field::FileId::Manufacturer,
         fit::field::FileId::Product, fit::field::FileId::SerialNumber,
         fit::field::FileId::TimeCreated,
         {fit::field::FileId::kProductNameNum, fit::BaseType::String, productNameLen}});
    mFit->data(L_FILE_ID)
        .u8(static_cast<uint8_t>(fit::File::Activity))
        .u16(static_cast<uint16_t>(fit::Manufacturer::Una))
        .u16(static_cast<uint16_t>(fit::Product::UnaWatch))
        .u32(0)
        .u32(unixToFitTimestamp(info.timestamp))
        .str(fit::kProductName, productNameLen)
        .write();

    // developer_data_id
    mFit->defineMessage(L_DEV_ID, fit::mesgNum(fit::MesgNum::DeveloperDataId),
        {fit::field::DeveloperDataId::ApplicationId,
         fit::field::DeveloperDataId::DeveloperDataIndex});
    {
        uint8_t appId[16] = {};
        std::strncpy(reinterpret_cast<char*>(appId), info.appID.c_str(), sizeof(appId));
        mFit->data(L_DEV_ID).bytes(appId, sizeof(appId)).u8(0).write();
    }

    // Developer fields
    writeFieldDescription(DF_BATTERY_LEVEL, "batteryLevel", "%", fit::BaseType::UInt8);
    writeFieldDescription(DF_WORKOUT_TITLE, "workout_title", nullptr, fit::BaseType::String);
    writeFieldDescription(DF_PATTERN_NAME, "pattern_name", nullptr, fit::BaseType::String);
    writeFieldDescription(DF_CYCLES, "cycles_completed", nullptr, fit::BaseType::UInt16);

    // event
    mFit->defineMessage(L_EVENT, fit::mesgNum(fit::MesgNum::Event),
        {fit::field::Event::Timestamp, fit::field::Event::EventField,
         fit::field::Event::EventType});

    defineRecordMessages();

    // session / activity
    mFit->defineMessage(L_SESSION, fit::mesgNum(fit::MesgNum::Session),
        {fit::field::Session::Timestamp, fit::field::Session::StartTime,
         fit::field::Session::TotalElapsedTime, fit::field::Session::TotalTimerTime,
         fit::field::Session::MessageIndex, fit::field::Session::NumLaps,
         fit::field::Session::Sport, fit::field::Session::SubSport,
         fit::field::Session::AvgHeartRate, fit::field::Session::MaxHeartRate,
         fit::field::Session::TotalCalories, fit::field::Session::MetabolicCalories});

    mFit->defineMessage(L_ACTIVITY, fit::mesgNum(fit::MesgNum::Activity),
        {fit::field::Activity::Timestamp, fit::field::Activity::TotalTimerTime,
         fit::field::Activity::LocalTimestamp, fit::field::Activity::NumSessions});

    addMessageEvent(info.timestamp, fit::EventType::Start);

    if (begun && mFile->flush()) {
        mMarker.write(mFile->getPath(), static_cast<uint32_t>(mFile->getPosition()));
    }
}

void ActivityWriter::defineRecordMessages()
{
    mFit->defineMessage(L_RECORD, fit::mesgNum(fit::MesgNum::Record),
        {fit::field::Record::Timestamp, fit::field::Record::HeartRate},
        {{DF_BATTERY_LEVEL, 1, 0}});
}

void ActivityWriter::writeFieldDescription(uint8_t devFieldNum, const char* name,
                                           const char* units, fit::BaseType baseType)
{
    const uint8_t nameLen  = name ? static_cast<uint8_t>(std::strlen(name) + 1) : 1;
    const uint8_t unitsLen = units ? static_cast<uint8_t>(std::strlen(units) + 1) : 1;

    mFit->defineMessage(L_FIELD_DESC, fit::mesgNum(fit::MesgNum::FieldDescription),
        {fit::field::FieldDescription::DeveloperDataIndex,
         fit::field::FieldDescription::FieldDefinitionNumber,
         fit::field::FieldDescription::FitBaseTypeId,
         {fit::field::FieldDescription::kFieldNameNum, fit::BaseType::String, nameLen},
         {fit::field::FieldDescription::kUnitsNum, fit::BaseType::String, unitsLen}});
    mFit->data(L_FIELD_DESC)
        .u8(0)
        .u8(devFieldNum)
        .u8(fit::baseTypeId(baseType))
        .str(name ? name : "", nameLen)
        .str(units ? units : "", unitsLen)
        .write();
}

void ActivityWriter::pause(std::time_t timestamp)
{
    if (mFit) {
        addMessageEvent(timestamp, fit::EventType::Stop);
    }
}

void ActivityWriter::resume(std::time_t timestamp)
{
    if (mFit) {
        addMessageEvent(timestamp, fit::EventType::Start);
    }
}

void ActivityWriter::addRecord(const RecordData& record)
{
    if (!mFit) {
        return;
    }

    fit::FitWriter::Data d = mFit->data(L_RECORD);
    d.u32(unixToFitTimestamp(record.timestamp));
    if (record.heartRate > 0.0f) {
        d.u8(static_cast<uint8_t>(record.heartRate + 0.5f));
    } else {
        d.u8(static_cast<uint8_t>(fit::baseTypeInvalid(fit::BaseType::UInt8)));
    }
    d.u8(record.batteryLevel);
    d.write();

    if (record.timestamp - mLastFlushUtc >= skFlushIntervalSec) {
        if (mFile->flush()) {
            mMarker.update(static_cast<uint32_t>(mFile->getPosition()));
            mLastFlushUtc = record.timestamp;
        }
    }
}

bool ActivityWriter::stop(const TrackData& track)
{
    if (!mFit) {
        return false;
    }

    bool ok = mFit->ok();

    // Define and write Workout message with workout title
    const uint8_t wktNameLen = static_cast<uint8_t>(track.workoutTitle.length() + 1);
    mFit->defineMessage(L_WORKOUT, fit::mesgNum(fit::MesgNum::Workout),
        {fit::field::Workout::Sport,
         fit::field::Workout::SubSport,
         fit::field::Workout::NumValidSteps,
         {fit::field::Workout::kWktNameNum, fit::BaseType::String, wktNameLen}});

    ok = mFit->data(L_WORKOUT)
        .u8(static_cast<uint8_t>(fit::Sport::Training))
        .u8(static_cast<uint8_t>(fit::SubSport::Generic))
        .u16(1)
        .str(track.workoutTitle.c_str(), wktNameLen)
        .write() && ok;

    // Session message
    ok = mFit->data(L_SESSION)
        .u32(unixToFitTimestamp(track.timestamp))
        .u32(unixToFitTimestamp(track.timeStart))
        .u32(static_cast<uint32_t>(track.elapsed * 1000))
        .u32(static_cast<uint32_t>(track.duration * 1000))
        .u16(0)  // message_index
        .u16(1)  // num_laps
        .u8(static_cast<uint8_t>(fit::Sport::Training))
        .u8(static_cast<uint8_t>(fit::SubSport::Generic))
        .u8(static_cast<uint8_t>(track.hrAvg + 0.5f))
        .u8(static_cast<uint8_t>(track.hrMax + 0.5f))
        .u16(static_cast<uint16_t>(track.calories + 0.5f))
        .u16(static_cast<uint16_t>(track.metabolicCalories + 0.5f))
        .write() && ok;

    // Activity message
    ok = mFit->data(L_ACTIVITY)
        .u32(unixToFitTimestamp(track.timestamp))
        .u32(static_cast<uint32_t>(track.duration * 1000))
        .u32(unixToFitTimestamp(epochToLocal(track.timestamp)))
        .u16(1)
        .write() && ok;

    const bool finishOk = mFit->finish();
    if (!finishOk) {
        LOG_ERROR("Failed to finalize FIT file\n");
    }
    ok = finishOk && mFit->ok() && ok;
    mFit.reset();

    if (mFile) {
        ok = mFile->flush() && ok;
        ok = mFile->close() && ok;
    } else {
        ok = false;
    }

    if (ok) {
        mMarker.remove();
        saveSummary(track);
    }
    return ok;
}

void ActivityWriter::discard()
{
    mFit.reset();
    mMarker.remove();
    if (!mFile) {
        return;
    }
    if (mFile->isOpen()) {
        mFile->close();
    }
    mFile->remove();
    mFile.reset();
}

void ActivityWriter::addMessageEvent(std::time_t t, fit::EventType type)
{
    mFit->data(L_EVENT)
        .u32(unixToFitTimestamp(t))
        .u8(static_cast<uint8_t>(fit::Event::Timer))
        .u8(static_cast<uint8_t>(type))
        .write();
}

bool ActivityWriter::recoverInterrupted()
{
    const auto result = mMarker.recover();
    if (result.recovered) {
        LOG_INFO("Recovery: finalized interrupted activity [%s]\n", result.path.c_str());
    }
    return result.recovered;
}

bool ActivityWriter::createAndOpenFile(std::time_t utc)
{
    char buff[256]{};
    std::tm localTime{};
#if WIN32
    localtime_s(&localTime, &utc);
#else
    localtime_r(&utc, &localTime);
#endif

    int len = snprintf(buff, sizeof(buff), "%s/%04u%02u/", mPath,
                       localTime.tm_year + 1900, localTime.tm_mon + 1);
    if (len <= 0 || !mKernel.fs.mkdir(buff)) {
        LOG_ERROR("Failed to create dir [%s]\n", buff);
        return false;
    }

    snprintf(&buff[len], sizeof(buff) - len, "activity_%04u%02u%02uT%02u%02u%02u.fit",
        localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday,
        localTime.tm_hour, localTime.tm_min, localTime.tm_sec);

    mFile = mKernel.fs.file(buff);
    if (!mFile || !mFile->open(true, true)) {
        LOG_ERROR("Failed to create file [%s]\n", buff);
        mFile.reset();
        return false;
    }

    return true;
}

bool ActivityWriter::saveSummary(const TrackData& track)
{
    if (!mFile) {
        return false;
    }
    char buff[256]{};
    size_t nameLen = strlen(mFile->getPath());
    snprintf(buff, sizeof(buff), "%.*s%s", static_cast<int>(nameLen - 3), mFile->getPath(), "json");

    mFile->setPath(buff);
    if (!mFile->open(true, true)) {
        LOG_ERROR("Failed to open activity summary [%s]\n", buff);
        mFile.reset();
        return false;
    }

    SDK::JsonStreamWriter writer(mFile.get());
    writer.startMap();
    writer.add("time_start", static_cast<uint32_t>(track.timeStart));
    writer.add("duration", static_cast<uint32_t>(track.duration));
    writer.add("hr_avg", track.hrAvg);
    writer.add("hr_max", track.hrMax);
    writer.add("hr_min", track.hrMin);
    writer.add("calories", track.calories);
    writer.add("activity_type", "workout");
    writer.add("activity_name", track.workoutTitle.c_str());
    writer.add("title", track.workoutTitle.c_str());
    writer.add("sport", "training");
    writer.add("pattern", track.patternName.c_str());
    writer.add("cycles", static_cast<uint32_t>(track.cycles));
    writer.endMap();

    const bool ok = mFile->flush();
    return mFile->close() && ok;
}

std::time_t ActivityWriter::tm2epoch(const struct tm* tm)
{
    int y = tm->tm_year + 1900;
    int m = tm->tm_mon + 1;
    int d = tm->tm_mday;
    if (m <= 2) { y -= 1; m += 12; }
    int64_t  era = (y >= 0 ? y : y - 399) / 400;
    uint32_t yoe = (uint32_t)(y - era * 400);
    uint32_t doy = (153 * (m - 3) + 2) / 5 + d - 1;
    uint32_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    int64_t days = era * 146097 + (int64_t)doe - 719468;
    int64_t secs = days * 86400 + tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;
    return (std::time_t)secs;
}

std::time_t ActivityWriter::epochToLocal(std::time_t utc)
{
    std::tm localTime{};
#if WIN32
    localtime_s(&localTime, &utc);
#else
    localtime_r(&utc, &localTime);
#endif
    return tm2epoch(&localTime);
}

uint32_t ActivityWriter::unixToFitTimestamp(std::time_t unixTimestamp)
{
    const std::time_t FIT_EPOCH_OFFSET = 631065600;
    return static_cast<uint32_t>(unixTimestamp - FIT_EPOCH_OFFSET);
}

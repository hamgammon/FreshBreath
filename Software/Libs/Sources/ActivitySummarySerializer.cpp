/**
 ******************************************************************************
 * @file    ActivitySummarySerializer.cpp
 * @brief   Serializes/Deserializes FreshBreath activity summary data.
 ******************************************************************************
 */

#include "ActivitySummarySerializer.hpp"

#include <cassert>
#include <cstring>
#include <new>

#include "SDK/JSON/JsonStreamWriter.hpp"
#include "SDK/JSON/JsonStreamReader.hpp"

#define LOG_MODULE_PRX      "SummarySerializer"
#define LOG_MODULE_LEVEL    LOG_LEVEL_INFO
#include "SDK/UnaLogger/Logger.h"

ActivitySummarySerializer::ActivitySummarySerializer(const SDK::Kernel& kernel,
                                                     const char* pathToFile)
    : mKernel(kernel), mPath(pathToFile)
{
    assert(pathToFile != nullptr);
}

bool ActivitySummarySerializer::save(const ActivitySummary& summary)
{
    const char* slash = std::strrchr(mPath, '/');
    if (slash) {
        char buff[SDK::Interface::IFileSystem::skMaxPathLen]{};
        std::snprintf(buff, sizeof(buff), "%.*s", static_cast<int>(slash - mPath), mPath);
        if (!mKernel.fs.mkdir(buff)) {
            return false;
        }
    }

    std::unique_ptr<SDK::Interface::IFile> file = mKernel.fs.file(mPath);
    if (!file || !file->open(true, true)) {
        return false;
    }

    SDK::JsonStreamWriter writer(file.get());
    writer.startMap();
    writer.add("utc", static_cast<uint32_t>(summary.utc));
    writer.add("duration", static_cast<uint32_t>(summary.duration));
    writer.add("hr_avg", summary.hrAvg);
    writer.add("hr_max", summary.hrMax);
    writer.add("hr_min", summary.hrMin);
    writer.add("calories", summary.calories);
    writer.add("cycles", static_cast<uint32_t>(summary.cycles));
    writer.add("pattern_name", summary.patternName);
    writer.add("workout_title", summary.workoutTitle);
    writer.endMap();

    file->flush();
    return file->close();
}

bool ActivitySummarySerializer::load(ActivitySummary& summary)
{
    std::unique_ptr<SDK::Interface::IFile> file = mKernel.fs.file(mPath);
    if (!file || !file->exist() || !file->open(false, false)) {
        return false;
    }

    size_t fileSize = file->size();
    if (fileSize == 0) {
        file->close();
        return false;
    }

    char* buffer = new (std::nothrow) char[fileSize];
    if (!buffer) {
        file->close();
        return false;
    }

    size_t read = 0;
    bool status = file->read(buffer, fileSize, read) && (read == fileSize);
    file->close();

    if (!status) {
        delete[] buffer;
        return false;
    }

    SDK::JsonStreamReader reader(buffer, fileSize);
    if (!reader.validate()) {
        delete[] buffer;
        return false;
    }

    uint32_t val32 = 0;
    if (reader.get("utc", val32)) summary.utc = static_cast<std::time_t>(val32);
    if (reader.get("duration", val32)) summary.duration = static_cast<std::time_t>(val32);
    reader.get("hr_avg", summary.hrAvg);
    reader.get("hr_max", summary.hrMax);
    reader.get("hr_min", summary.hrMin);
    reader.get("calories", summary.calories);
    if (reader.get("cycles", val32)) summary.cycles = static_cast<uint16_t>(val32);

    const char* strVal = nullptr;
    size_t strLen = 0;
    if (reader.get("pattern_name", strVal, strLen) && strVal) {
        std::snprintf(summary.patternName, sizeof(summary.patternName), "%.*s", static_cast<int>(strLen), strVal);
    }
    if (reader.get("workout_title", strVal, strLen) && strVal) {
        std::snprintf(summary.workoutTitle, sizeof(summary.workoutTitle), "%.*s", static_cast<int>(strLen), strVal);
    }

    delete[] buffer;
    return true;
}

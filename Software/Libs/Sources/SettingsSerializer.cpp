/**
 ******************************************************************************
 * @file    SettingsSerializer.cpp
 * @brief   Serializes/Deserializes FreshBreath settings to JSON.
 ******************************************************************************
 */

#include "SettingsSerializer.hpp"

#include <cassert>
#include <cstring>
#include <new>

#include "SDK/JSON/JsonStreamWriter.hpp"
#include "SDK/JSON/JsonStreamReader.hpp"
#include "BreathPatterns.hpp"

#define LOG_MODULE_PRX      "SettingsSerializer"
#define LOG_MODULE_LEVEL    LOG_LEVEL_INFO
#include "SDK/UnaLogger/Logger.h"

SettingsSerializer::SettingsSerializer(const SDK::Kernel& kernel, const char* pathToFile)
    : mKernel(kernel), mPath(pathToFile)
{
    assert(pathToFile != nullptr);
}

bool SettingsSerializer::save(const Settings& settings)
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
    writer.add("pattern_index", settings.selectedPatternIndex);
    writer.add("target_duration", settings.targetDurationSec);
    writer.add("vibrate_only", settings.vibrateOnly);
    writer.endMap();

    file->flush();
    return file->close();
}

bool SettingsSerializer::load(Settings& settings)
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

    uint8_t pIndex = 0;
    if (reader.get("pattern_index", pIndex) && pIndex < FreshBreath::PatternRegistry::kPatternCount) {
        settings.selectedPatternIndex = pIndex;
    }

    uint16_t dur = 300;
    if (reader.get("target_duration", dur)) {
        settings.targetDurationSec = dur;
    }

    bool vibOnly = false;
    if (reader.get("vibrate_only", vibOnly)) {
        settings.vibrateOnly = vibOnly;
    }

    delete[] buffer;
    return true;
}

/**
 ******************************************************************************
 * @file    SettingsSerializer.hpp
 * @brief   Serializes/Deserializes FreshBreath settings.
 ******************************************************************************
 */

#ifndef SETTINGS_SERIALIZER_HPP
#define SETTINGS_SERIALIZER_HPP

#include "SDK/Kernel/Kernel.hpp"
#include "Settings.hpp"

class SettingsSerializer {
public:
    SettingsSerializer(const SDK::Kernel& kernel, const char* pathToFile);
    ~SettingsSerializer() = default;

    bool save(const Settings& settings);
    bool load(Settings& settings);

private:
    const SDK::Kernel& mKernel;
    const char* mPath;
};

#endif // SETTINGS_SERIALIZER_HPP

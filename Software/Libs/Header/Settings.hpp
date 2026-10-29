/**
 ******************************************************************************
 * @file    Settings.hpp
 * @brief   Persistent user settings for FreshBreath.
 ******************************************************************************
 */

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <cstdint>

struct Settings {
    uint8_t  selectedPatternIndex = 0;   ///< Default: Box Breathing 4-4-4-4
    uint16_t targetDurationSec    = 300; ///< Target duration in seconds (0 = open)
    bool     hapticsEnabled       = true;///< Haptic vibration on breath phase changes

    bool operator==(const Settings& other) const {
        return selectedPatternIndex == other.selectedPatternIndex &&
               targetDurationSec == other.targetDurationSec &&
               hapticsEnabled == other.hapticsEnabled;
    }
};

#endif // SETTINGS_HPP

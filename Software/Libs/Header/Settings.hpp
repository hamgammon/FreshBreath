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
    uint8_t  selectedPatternIndex = 0;     ///< Default: Box Breathing 4-4-4-4
    uint16_t targetDurationSec    = 300;   ///< Target duration in seconds (0 = open)
    bool     vibrateOnly          = false; ///< false = Sound & Vibrate, true = Vibration Only

    bool operator==(const Settings& other) const {
        return selectedPatternIndex == other.selectedPatternIndex &&
               targetDurationSec == other.targetDurationSec &&
               vibrateOnly == other.vibrateOnly;
    }
};

#endif // SETTINGS_HPP

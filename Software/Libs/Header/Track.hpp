/**
 ******************************************************************************
 * @file    Track.hpp
 * @brief   Track namespace: state and real-time metrics for FreshBreath.
 ******************************************************************************
 */

#ifndef TRACK_HPP
#define TRACK_HPP

#include <cstdint>
#include <ctime>
#include "BreathPatterns.hpp"

namespace Track {

enum class State : uint8_t {
    INACTIVE = 0,
    ACTIVE,
    PAUSED,
    COMPLETED
};

struct Data {
    std::time_t totalTime   = 0;
    std::time_t targetTime  = 0;

    FreshBreath::BreathPhase phase = FreshBreath::BreathPhase::INHALE;
    uint16_t phaseElapsedSec       = 0;
    uint16_t phaseDurationSec      = 4;
    uint16_t completedCycles       = 0;

    float hr           = 0.0f;
    float hrAvg        = 0.0f;
    float hrMax        = 0.0f;
    float hrMin        = 0.0f;
    float totalCalories= 0.0f;

    uint8_t patternIndex = 0;
};

} // namespace Track

#endif // TRACK_HPP

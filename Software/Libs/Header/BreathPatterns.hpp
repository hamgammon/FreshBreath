/**
 ******************************************************************************
 * @file    BreathPatterns.hpp
 * @brief   Predefined breathwork patterns, ratios, and timing configurations.
 ******************************************************************************
 */

#ifndef BREATH_PATTERNS_HPP
#define BREATH_PATTERNS_HPP

#include <cstdint>
#include <cstddef>

namespace FreshBreath {

enum class BreathPhase : uint8_t {
    INHALE = 0,
    HOLD_IN,
    EXHALE,
    HOLD_OUT
};

struct BreathPattern {
    const char* id;
    const char* displayName;      ///< Menu display label
    const char* workoutTitle;     ///< Saved workout title e.g. "Breathwork - 4:4:4:4 Method"
    const char* ratioLabel;       ///< Ratio e.g. "4:4:4:4"
    uint16_t    inhaleSec;
    uint16_t    holdInSec;
    uint16_t    exhaleSec;
    uint16_t    holdOutSec;
    uint16_t    defaultDurationSec;

    uint16_t cycleDurationSec() const {
        return inhaleSec + holdInSec + exhaleSec + holdOutSec;
    }
};

class PatternRegistry {
public:
    static constexpr size_t kPatternCount = 6;
    static const BreathPattern kPatterns[kPatternCount];

    static const BreathPattern& getPattern(size_t index);
    static const BreathPattern* findById(const char* id);
    static const char* phaseName(BreathPhase phase);
};

} // namespace FreshBreath

#endif // BREATH_PATTERNS_HPP

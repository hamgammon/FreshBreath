/**
 ******************************************************************************
 * @file    BreathPatterns.cpp
 * @brief   Registry of validated breathwork patterns and phase definitions.
 ******************************************************************************
 */

#include "BreathPatterns.hpp"
#include <cstring>

namespace FreshBreath {

const BreathPattern PatternRegistry::kPatterns[PatternRegistry::kPatternCount] = {
    {
        "box_4444",
        "Box 4-4-4-4",
        "Breathwork - 4:4:4:4 Method",
        "4:4:4:4",
        4, 4, 4, 4,
        300 // 5 minutes
    },
    {
        "relax_478",
        "4-7-8 Relax",
        "Breathwork - 4:7:8 Method",
        "4:7:8:0",
        4, 7, 8, 0,
        240 // 4 minutes
    },
    {
        "coherent_55",
        "Resonance 5:5",
        "Breathwork - 5:5 Coherent",
        "5:0:5:0",
        5, 0, 5, 0,
        600 // 10 minutes (HRV optimum)
    },
    {
        "deep_calm_48",
        "Deep Calm 4:8",
        "Breathwork - 4:8 Method",
        "4:0:8:0",
        4, 0, 8, 0,
        300 // 5 minutes
    },
    {
        "awake_412",
        "Energize 4:1:2",
        "Breathwork - 4:1:2 Method",
        "4:1:2:0",
        4, 1, 2, 0,
        180 // 3 minutes
    },
    {
        "equal_44",
        "Equal Flow 4:4",
        "Breathwork - 4:4 Method",
        "4:0:4:0",
        4, 0, 4, 0,
        300 // 5 minutes
    }
};

const BreathPattern& PatternRegistry::getPattern(size_t index)
{
    if (index >= kPatternCount) {
        return kPatterns[0];
    }
    return kPatterns[index];
}

const BreathPattern* PatternRegistry::findById(const char* id)
{
    if (!id) return &kPatterns[0];
    for (size_t i = 0; i < kPatternCount; ++i) {
        if (std::strcmp(kPatterns[i].id, id) == 0) {
            return &kPatterns[i];
        }
    }
    return &kPatterns[0];
}

const char* PatternRegistry::phaseName(BreathPhase phase)
{
    switch (phase) {
        case BreathPhase::INHALE:   return "INHALE";
        case BreathPhase::HOLD_IN:  return "HOLD";
        case BreathPhase::EXHALE:   return "EXHALE";
        case BreathPhase::HOLD_OUT: return "HOLD";
        default:                    return "";
    }
}

} // namespace FreshBreath

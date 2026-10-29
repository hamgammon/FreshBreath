/**
 ******************************************************************************
 * @file    ActivitySummary.hpp
 * @brief   Summary data for a completed FreshBreath session.
 ******************************************************************************
 */

#ifndef ACTIVITY_SUMMARY_HPP
#define ACTIVITY_SUMMARY_HPP

#include <cstdint>
#include <ctime>

struct ActivitySummary {
    std::time_t utc          = 0;     ///< UTC start timestamp
    std::time_t duration     = 0;     ///< Total elapsed time in seconds
    float       hrAvg        = 0.0f;  ///< Average Heart Rate in bpm
    float       hrMax        = 0.0f;  ///< Maximum Heart Rate in bpm
    float       hrMin        = 0.0f;  ///< Minimum Heart Rate in bpm
    float       calories     = 0.0f;  ///< Estimated calories (kcal)
    uint16_t    cycles       = 0;     ///< Completed breath cycles
    char        patternName[32]  = {};///< Short pattern name e.g. "Box 4-4-4-4"
    char        workoutTitle[48] = {};///< Title e.g. "Breathwork - 4:4:4:4 Method"
};

#endif // ACTIVITY_SUMMARY_HPP

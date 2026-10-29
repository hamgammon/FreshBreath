/**
 ******************************************************************************
 * @file    ModelListener.hpp
 * @brief   Interface for screens listening to model updates.
 ******************************************************************************
 */

#ifndef MODEL_LISTENER_HPP
#define MODEL_LISTENER_HPP

#include "Track.hpp"
#include "ActivitySummary.hpp"
#include "Settings.hpp"

class ModelListener {
public:
    virtual ~ModelListener() = default;

    virtual void onTrackStateChanged(Track::State state) { (void)state; }
    virtual void onTrackDataUpdated(const Track::Data& data) { (void)data; }
    virtual void onSettingsUpdated(const Settings& settings) { (void)settings; }
    virtual void onSummaryUpdated(const ActivitySummary& summary) { (void)summary; }
    virtual void onTimeUpdated(const std::tm& time) { (void)time; }
    virtual void onBatteryUpdated(uint8_t level) { (void)level; }
};

#endif // MODEL_LISTENER_HPP

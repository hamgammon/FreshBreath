/**
 ******************************************************************************
 * @file    Model.hpp
 * @brief   GUI state model and link to the FreshBreath service process.
 ******************************************************************************
 */

#ifndef MODEL_HPP
#define MODEL_HPP

#include <ctime>
#include <cstdint>

#include "SDK/Kernel/Kernel.hpp"
#include "SDK/Port/LVGL/LvglPort.hpp"
#include "SDK/Interfaces/IGuiLifeCycleCallback.hpp"
#include "SDK/Interfaces/ICustomMessageHandler.hpp"

#include "Commands.hpp"
#include "Settings.hpp"
#include "Track.hpp"
#include "ActivitySummary.hpp"
#include "ModelListener.hpp"

class Model : public SDK::Interface::IGuiLifeCycleCallback,
              public SDK::Interface::ICustomMessageHandler
{
public:
    Model();
    ~Model() override;

    void setListener(ModelListener* listener) { mListener = listener; }

    // Service requests
    void startTrack(uint8_t patternIndex, uint16_t durationSec);
    void pauseTrack();
    void resumeTrack();
    void stopTrack(bool discard = false);
    void saveSettings(const Settings& settings);
    void exitApp();

    // Getters
    Track::State getTrackState() const { return mTrackState; }
    const Track::Data& getTrackData() const { return mTrackData; }
    const Settings& getSettings() const { return mSettings; }
    const ActivitySummary& getSummary() const { return mSummary; }
    const std::tm& getTime() const { return mTime; }
    uint8_t getBatteryLevel() const { return mBatteryLevel; }
    bool is12Hour() const { return mTimeFormat12h; }

    // Key event from screen
    void handleKeyEvent(uint8_t key);

    // ICustomMessageHandler
    bool customMessageHandler(SDK::MessageBase* message) override;

private:
    ModelListener* mListener = nullptr;
    const SDK::Kernel& mKernel;

    Track::State    mTrackState = Track::State::INACTIVE;
    Track::Data     mTrackData{};
    Settings        mSettings{};
    ActivitySummary mSummary{};
    std::tm         mTime{};
    uint8_t         mBatteryLevel = 100;
    bool            mTimeFormat12h = false;
};

#endif // MODEL_HPP

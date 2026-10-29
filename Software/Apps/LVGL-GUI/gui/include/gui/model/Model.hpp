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
#include "SDK/GUI/Button.hpp"

#include "Commands.hpp"
#include "Settings.hpp"
#include "Track.hpp"
#include "ActivitySummary.hpp"
#include "ModelListener.hpp"

namespace Btn = SDK::GUI::Button;

class Model : public SDK::Interface::IGuiLifeCycleCallback,
              public SDK::Interface::ICustomMessageHandler
{
public:
    enum class ResultMode : uint8_t {
        SettingSaved = 0,
        WorkoutSaved,
        WorkoutDiscarded
    };

    enum class HoldConfirmMode : uint8_t {
        Finish = 0,
        Discard
    };

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

    // UI cursor and navigation state
    uint16_t getMainCursor() const { return mMainCursor; }
    void setMainCursor(uint16_t cursor) { mMainCursor = cursor; }

    uint16_t getPauseCursor() const { return mPauseCursor; }
    void setPauseCursor(uint16_t cursor) { mPauseCursor = cursor; }

    ResultMode getResultMode() const { return mResultMode; }
    void setResultMode(ResultMode mode) { mResultMode = mode; }

    HoldConfirmMode getHoldConfirmMode() const { return mHoldConfirmMode; }
    void setHoldConfirmMode(HoldConfirmMode mode) { mHoldConfirmMode = mode; }

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

    uint16_t        mMainCursor = 0;
    uint16_t        mPauseCursor = 0;
    ResultMode      mResultMode = ResultMode::SettingSaved;
    HoldConfirmMode mHoldConfirmMode = HoldConfirmMode::Finish;
};

#endif // MODEL_HPP

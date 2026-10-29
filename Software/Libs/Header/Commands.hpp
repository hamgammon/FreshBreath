/**
 ******************************************************************************
 * @file    Commands.hpp
 * @brief   Custom IPC messages between Service and GUI for FreshBreath.
 ******************************************************************************
 */

#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <cstring>
#include <ctime>

#include "SDK/Messages/MessageBase.hpp"
#include "SDK/Messages/MessageTypes.hpp"
#include "Settings.hpp"
#include "Track.hpp"
#include "ActivitySummary.hpp"

#pragma pack(push, 4)

namespace CustomMessage {

constexpr SDK::MessageType::Type SETTINGS_UPDATE    = 0x00000001;
constexpr SDK::MessageType::Type LOCAL_TIME         = 0x00000002;
constexpr SDK::MessageType::Type BATTERY            = 0x00000003;
constexpr SDK::MessageType::Type TRACK_STATE_UPDATE = 0x00000005;
constexpr SDK::MessageType::Type TRACK_DATA_UPDATE  = 0x00000006;
constexpr SDK::MessageType::Type SUMMARY            = 0x00000008;

constexpr SDK::MessageType::Type SETTINGS_SAVE      = 0x0000000A;
constexpr SDK::MessageType::Type TRACK_START        = 0x0000000B;
constexpr SDK::MessageType::Type TRACK_STOP         = 0x0000000C;
constexpr SDK::MessageType::Type TRACK_PAUSE        = 0x0000000D;
constexpr SDK::MessageType::Type TRACK_RESUME       = 0x0000000E;
constexpr SDK::MessageType::Type TRACK_DISCARD      = 0x0000000F;

struct SettingsUpd : public SDK::MessageBase {
    Settings settings;
    bool timeFormat12h;

    SettingsUpd()
        : SDK::MessageBase(SETTINGS_UPDATE)
        , settings{}
        , timeFormat12h(false)
    {}

    explicit SettingsUpd(const Settings& s, bool tf12)
        : SDK::MessageBase(SETTINGS_UPDATE)
        , settings(s)
        , timeFormat12h(tf12)
    {}
};

struct SettingsSave : public SDK::MessageBase {
    Settings settings;

    SettingsSave()
        : SDK::MessageBase(SETTINGS_SAVE)
        , settings{}
    {}

    explicit SettingsSave(const Settings& s)
        : SDK::MessageBase(SETTINGS_SAVE)
        , settings(s)
    {}
};

struct TrackStateUpd : public SDK::MessageBase {
    Track::State state;

    TrackStateUpd()
        : SDK::MessageBase(TRACK_STATE_UPDATE)
        , state(Track::State::INACTIVE)
    {}

    explicit TrackStateUpd(Track::State s)
        : SDK::MessageBase(TRACK_STATE_UPDATE)
        , state(s)
    {}
};

struct TrackDataUpd : public SDK::MessageBase {
    Track::Data data;

    TrackDataUpd()
        : SDK::MessageBase(TRACK_DATA_UPDATE)
        , data{}
    {}

    explicit TrackDataUpd(const Track::Data& d)
        : SDK::MessageBase(TRACK_DATA_UPDATE)
        , data(d)
    {}
};

struct TrackStartReq : public SDK::MessageBase {
    uint8_t  patternIndex;
    uint16_t targetDuration;

    TrackStartReq()
        : SDK::MessageBase(TRACK_START)
        , patternIndex(0)
        , targetDuration(300)
    {}

    explicit TrackStartReq(uint8_t idx, uint16_t dur)
        : SDK::MessageBase(TRACK_START)
        , patternIndex(idx)
        , targetDuration(dur)
    {}
};

struct TrackStopReq : public SDK::MessageBase {
    TrackStopReq() : SDK::MessageBase(TRACK_STOP) {}
};

struct TrackPauseReq : public SDK::MessageBase {
    TrackPauseReq() : SDK::MessageBase(TRACK_PAUSE) {}
};

struct TrackResumeReq : public SDK::MessageBase {
    TrackResumeReq() : SDK::MessageBase(TRACK_RESUME) {}
};

struct TrackDiscardReq : public SDK::MessageBase {
    TrackDiscardReq() : SDK::MessageBase(TRACK_DISCARD) {}
};

struct SummaryUpd : public SDK::MessageBase {
    ActivitySummary summary;

    SummaryUpd()
        : SDK::MessageBase(SUMMARY)
        , summary{}
    {}

    explicit SummaryUpd(const ActivitySummary& sum)
        : SDK::MessageBase(SUMMARY)
        , summary(sum)
    {}
};

struct Time : public SDK::MessageBase {
    std::tm localTime;

    Time()
        : SDK::MessageBase(LOCAL_TIME)
        , localTime{}
    {}

    explicit Time(const std::tm& t)
        : SDK::MessageBase(LOCAL_TIME)
        , localTime(t)
    {}
};

struct Battery : public SDK::MessageBase {
    uint8_t level;

    Battery()
        : SDK::MessageBase(BATTERY)
        , level(0)
    {}

    explicit Battery(uint8_t lvl)
        : SDK::MessageBase(BATTERY)
        , level(lvl)
    {}
};

} // namespace CustomMessage

#pragma pack(pop)

#endif // COMMANDS_HPP

/**
 ******************************************************************************
 * @file    ScreenManager.hpp
 * @brief   Manages screen lifecycle and transitions in FreshBreath.
 ******************************************************************************
 */

#ifndef SCREEN_MANAGER_HPP
#define SCREEN_MANAGER_HPP

#include <cstdint>

class Model;
class Screen;

enum class ScreenId : uint8_t {
    Main = 0,
    PatternSelect,
    DurationSelect,
    Breath,
    Summary
};

class ScreenManager {
public:
    static ScreenManager& instance();

    void start(Model& model, ScreenId first);
    void goTo(ScreenId id);
    ScreenId currentId() const { return mCurrentId; }

private:
    ScreenManager() = default;
    void switchNow(ScreenId id);
    Screen* create(ScreenId id);
    static void asyncCb(void* user);

    Model*   mModel = nullptr;
    Screen*  mCurrent = nullptr;
    ScreenId mCurrentId = ScreenId::Main;
    ScreenId mPending = ScreenId::Main;
    bool     mPendingSet = false;
};

#endif // SCREEN_MANAGER_HPP

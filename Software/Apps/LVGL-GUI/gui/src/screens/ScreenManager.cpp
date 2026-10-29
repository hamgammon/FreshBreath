/**
 ******************************************************************************
 * @file    ScreenManager.cpp
 * @brief   Manages screen transitions and lifecycle for FreshBreath.
 ******************************************************************************
 */

#include "gui/screens/ScreenManager.hpp"
#include "lvgl.h"

#define LOG_MODULE_PRX      "ScreenManager"
#define LOG_MODULE_LEVEL    LOG_LEVEL_INFO
#include "SDK/UnaLogger/Logger.h"

#include "gui/model/Model.hpp"
#include "gui/screens/Screen.hpp"
#include "gui/screens/MainScreen.hpp"
#include "gui/screens/PatternSelectScreen.hpp"
#include "gui/screens/DurationSelectScreen.hpp"
#include "gui/screens/CountdownScreen.hpp"
#include "gui/screens/BreathScreen.hpp"
#include "gui/screens/PauseScreen.hpp"
#include "gui/screens/SummaryScreen.hpp"

ScreenManager& ScreenManager::instance()
{
    static ScreenManager sInstance;
    return sInstance;
}

void ScreenManager::start(Model& model, ScreenId first)
{
    mModel = &model;
    switchNow(first);
}

void ScreenManager::goTo(ScreenId id)
{
    mPending = id;
    if (!mPendingSet) {
        if (lv_async_call(&ScreenManager::asyncCb, this) != LV_RESULT_OK) {
            LOG_ERROR("Screen switch to %u failed to schedule\n", static_cast<unsigned>(id));
            return;
        }
        mPendingSet = true;
    }
}

void ScreenManager::asyncCb(void* user)
{
    auto* self = static_cast<ScreenManager*>(user);
    self->mPendingSet = false;
    self->switchNow(self->mPending);
}

Screen* ScreenManager::create(ScreenId id)
{
    if (!mModel) return nullptr;

    switch (id) {
        case ScreenId::Main:           return new MainScreen(*mModel);
        case ScreenId::PatternSelect:  return new PatternSelectScreen(*mModel);
        case ScreenId::DurationSelect: return new DurationSelectScreen(*mModel);
        case ScreenId::Countdown:      return new CountdownScreen(*mModel);
        case ScreenId::Breath:         return new BreathScreen(*mModel);
        case ScreenId::Pause:          return new PauseScreen(*mModel);
        case ScreenId::Summary:        return new SummaryScreen(*mModel);
        default:                       return nullptr;
    }
}

void ScreenManager::switchNow(ScreenId id)
{
    Screen* next = create(id);
    if (!next) {
        LOG_WARNING("Could not instantiate screen %u\n", static_cast<unsigned>(id));
        return;
    }

    Screen* old = mCurrent;
    if (old) {
        old->onHide();
        mModel->setListener(nullptr);
    }

    next->create();
    lv_screen_load(next->root());
    mCurrent = next;
    mCurrentId = id;

    if (old) {
        delete old;
    }

    mModel->setListener(next);
    next->onShow();

    LOG_INFO("Switched to screen %u\n", static_cast<unsigned>(id));
}

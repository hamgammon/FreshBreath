/**
 ******************************************************************************
 * @file    Widgets.hpp
 * @brief   Shared LVGL widgets for FreshBreath (Title, Buttons, WheelMenu,
 *          TimerRing, ScrollIndicator, Toggle, PauseIndicator, InfoCarousel).
 ******************************************************************************
 */

#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <cstdint>
#include <ctime>
#include <memory>

#include "lvgl.h"

#include "SDK/GUI/LVGL/Battery.hpp"
#include "SDK/GUI/LVGL/Buttons.hpp"
#include "SDK/GUI/LVGL/ScrollIndicator.hpp"
#include "SDK/GUI/LVGL/TimerRing.hpp"
#include "SDK/GUI/LVGL/Title.hpp"
#include "SDK/GUI/LVGL/Toggle.hpp"
#include "SDK/GUI/LVGL/WheelMenu.hpp"
#include "gui/theme/Theme.hpp"

namespace Widgets
{

using SDK::LVGL::kCx;
using SDK::LVGL::kCy;

using Buttons         = SDK::LVGL::Buttons;
using ScrollIndicator = SDK::LVGL::ScrollIndicator;
using Battery         = SDK::LVGL::Battery;
using Toggle          = SDK::LVGL::Toggle;

class TimerRing : public SDK::LVGL::TimerRing
{
public:
    TimerRing(lv_obj_t* parent, uint32_t color)
        : SDK::LVGL::TimerRing(parent)
    {
        setColor(color);
    }
};

class Title : public SDK::LVGL::Title
{
public:
    Title(lv_obj_t* parent, const char* text)
        : SDK::LVGL::Title(parent, Theme::font(Theme::Font::Italic18), text)
    {
    }
};

class WheelMenu : public SDK::LVGL::WheelMenu
{
public:
    using Style = Item::Style;
    WheelMenu(lv_obj_t* parent, const Item* items, uint16_t count, int16_t itemOffsetY = 0);
};

class PauseIndicator
{
public:
    PauseIndicator(lv_obj_t* parent, int32_t y);
    void setTime(std::time_t seconds);

private:
    lv_obj_t* mLabel = nullptr;
};

class InfoCarousel
{
public:
    using UpdateCb = void (*)(void* user, int16_t index);

    InfoCarousel(lv_obj_t* parent, int32_t x, int32_t y);
    ~InfoCarousel();

    void setPeriodMs(uint32_t ms);
    void setCallback(UpdateCb cb, void* user);
    void setCount(uint16_t count);
    void refresh();

    void setTitle(const char* text);
    void setValue(const char* text);

private:
    static void tickCb(lv_timer_t* t);
    void fire();

    lv_obj_t*   mTitle = nullptr;
    lv_obj_t*   mValue = nullptr;
    lv_timer_t* mTimer = nullptr;
    UpdateCb    mCb    = nullptr;
    void*       mUser  = nullptr;
    uint16_t    mCount = 0;
    uint16_t    mIndex = 0;
    uint32_t    mPeriodMs = 3000;
};

} // namespace Widgets

#endif // WIDGETS_HPP

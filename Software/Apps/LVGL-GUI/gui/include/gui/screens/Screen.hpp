/**
 ******************************************************************************
 * @file    Screen.hpp
 * @brief   Base class for FreshBreath GUI screens.
 ******************************************************************************
 */

#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <cstdint>
#include "lvgl.h"

#include "gui/model/Model.hpp"
#include "gui/model/ModelListener.hpp"
#include "SDK/GUI/Button.hpp"
#include "SDK/GUI/LVGL/Buttons.hpp"

class Screen : public ModelListener {
public:
    explicit Screen(Model& model);
    ~Screen() override;

    Screen(const Screen&) = delete;
    Screen& operator=(const Screen&) = delete;

    void create();
    void destroy();

    lv_obj_t* root() const { return mRoot; }

    virtual void onShow() {}
    virtual void onHide() {}
    virtual void onKey(uint8_t code) { (void)code; }

protected:
    virtual void build() = 0;

    Model&    mModel;
    lv_obj_t* mRoot = nullptr;

private:
    static void keyEventCb(lv_event_t* e);
};

#endif // SCREEN_HPP

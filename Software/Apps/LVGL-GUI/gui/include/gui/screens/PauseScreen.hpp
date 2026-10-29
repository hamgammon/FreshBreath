/**
 ******************************************************************************
 * @file    PauseScreen.hpp
 * @brief   Track pause action screen offering Resume, Save, or Discard.
 ******************************************************************************
 */

#ifndef PAUSE_SCREEN_HPP
#define PAUSE_SCREEN_HPP

#include "gui/screens/Screen.hpp"
#include "SDK/GUI/LVGL/Buttons.hpp"

#include <memory>
#include <cstdint>

class PauseScreen : public Screen {
public:
    enum class Action : uint8_t {
        Resume = 0,
        Save,
        Discard,
        Count
    };

    explicit PauseScreen(Model& model);
    ~PauseScreen() override = default;

    void build() override;
    void onShow() override;
    void onKey(uint8_t code) override;
    void onTrackStateChanged(Track::State state) override;

private:
    void updateHighlight();
    void executeSelectedAction();

    Action mSelected = Action::Resume;

    lv_obj_t* mTitleLabel = nullptr;
    lv_obj_t* mStatsLabel = nullptr;
    lv_obj_t* mCards[3]{};
    lv_obj_t* mLabels[3]{};
    lv_obj_t* mNavLabel = nullptr;

    std::unique_ptr<SDK::LVGL::Buttons> mButtons;
};

#endif // PAUSE_SCREEN_HPP

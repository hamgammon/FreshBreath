/**
 ******************************************************************************
 * @file    PauseScreen.hpp
 * @brief   Paused activity menu screen (Resume, Save & End, Discard)
 *          modeled after RunLVGL's TrackActionScreen.
 ******************************************************************************
 */

#ifndef PAUSE_SCREEN_HPP
#define PAUSE_SCREEN_HPP

#include <memory>
#include "gui/screens/Screen.hpp"
#include "gui/widgets/Widgets.hpp"

class PauseScreen : public Screen {
public:
    explicit PauseScreen(Model& model);
    ~PauseScreen() override = default;

    void onShow() override;
    void onHide() override;
    void onKey(uint8_t code) override;
    void onTrackDataUpdated(const Track::Data& data) override;

protected:
    void build() override;

private:
    static constexpr uint16_t kItemCount = 3;

    enum ItemIdx : uint16_t {
        ITEM_RESUME = 0,
        ITEM_SAVE_END,
        ITEM_DISCARD
    };

    enum Slot : int16_t {
        SLOT_CYCLES = 0,
        SLOT_AVG_HR,
        SLOT_CALORIES,
        SLOT_COUNT
    };

    static void carouselCb(void* user, int16_t index);
    void populateSlot(int16_t index);

    Widgets::WheelMenu::Item mItems[kItemCount]{};

    std::unique_ptr<Widgets::WheelMenu>      mMenu;
    std::unique_ptr<Widgets::InfoCarousel>   mInfo;
    std::unique_ptr<Widgets::PauseIndicator> mPause;
    std::unique_ptr<Widgets::Buttons>        mButtons;
};

#endif // PAUSE_SCREEN_HPP

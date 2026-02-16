#pragma once
/**
 * @file smartwatch_sports.hpp
 * @brief Sports screen for smartwatch demo
 */

#include "smartwatch_private.hpp"

// Font/Image declarations (must be outside namespace - C symbols)
LV_FONT_DECLARE(font_inter_bold_28);
LV_FONT_DECLARE(font_inter_bold_57);
LV_IMAGE_DECLARE(image_mask_gradient);

namespace smartwatch {

class SportsScreen {
public:
    void create(DemoController& controller);
    [[nodiscard]] lv_obj_t* get() const { return m_screen; }

private:
    void on_gesture(lv::Event e);
    void animate_arcs(int32_t v);

    lv_obj_t* m_screen = nullptr;
    lv_obj_t* m_arc_steps = nullptr;
    lv_obj_t* m_arc_distance = nullptr;
    lv_obj_t* m_arc_calories = nullptr;
    lv_obj_t* m_bar_steps = nullptr;
    lv_obj_t* m_bar_distance = nullptr;
    lv_obj_t* m_bar_calories = nullptr;
    lv_obj_t* m_label_steps = nullptr;
    lv_obj_t* m_label_distance = nullptr;
    lv_obj_t* m_label_calories = nullptr;
    lv_obj_t* m_label_time = nullptr;

    DemoController* m_controller = nullptr;
    lv::Style m_main_style;
    bool m_inited = false;
};

inline void SportsScreen::create(DemoController& controller) {
    m_controller = &controller;

    if (!m_inited) {
        m_main_style.text_color(lv::colors::white())
            .bg_color(lv::rgb(0x000000))
            .bg_opa(lv::opa::_100)
            .radius(lv::kRadius::circle)
            .translate_x(SCREEN_SIZE);
        m_inited = true;
    }

    auto screen = lv::Box::create(lv::screen_active());
    screen.remove_all_styles()
        .add_style(m_main_style.get())
        .fill()
        .scrollbar_mode(lv::kScrollbarMode::off)
        .remove_flag(lv::kFlag::gesture_bubble)
        .remove_flag(lv::kFlag::event_bubble);
    screen.on<&SportsScreen::on_gesture>(lv::kEvent::gesture, this);
    m_screen = screen.get();

    // Steps arc
    m_arc_steps = lv::Arc::create(m_screen)
        .size(246, 246)
        .pos(115, 80)
        .value(100)
        .bg_angles(270, 199)
        .arc_color(lv::rgb(0x2F1C25))
        .arc_opa(lv::opa::cover)
        .arc_width(23)
        .remove_flag(lv::kFlag::clickable)
        .indicator_arc_color(lv::rgb(0xF697C4))
        .indicator_arc_opa(lv::opa::cover)
        .indicator_arc_width(23)
        .knob_bg_color(lv::rgb(0xFFFFFF))
        .knob_bg_opa(lv::opa::transp)
        .get();

    // Distance arc
    m_arc_distance = lv::Arc::create(m_screen)
        .size(200, 200)
        .pos(138, 103)
        .value(50)
        .bg_angles(270, 205)
        .arc_color(lv::rgb(0x0E301A))
        .arc_opa(lv::opa::cover)
        .arc_width(23)
        .remove_flag(lv::kFlag::clickable)
        .indicator_arc_color(lv::rgb(0x4CFE8D))
        .indicator_arc_opa(lv::opa::cover)
        .indicator_arc_width(23)
        .knob_bg_opa(lv::opa::transp)
        .get();

    // Calories arc
    m_arc_calories = lv::Arc::create(m_screen)
        .size(155, 156)
        .pos(161, 126)
        .value(50)
        .bg_angles(270, 215)
        .arc_color(lv::rgb(0x112E2E))
        .arc_opa(lv::opa::cover)
        .arc_width(23)
        .remove_flag(lv::kFlag::clickable)
        .indicator_arc_color(lv::rgb(0x5CF2F1))
        .indicator_arc_opa(lv::opa::cover)
        .indicator_arc_width(23)
        .knob_bg_opa(lv::opa::transp)
        .get();

    // Steps bar
    m_bar_steps = lv::Bar::create(m_screen)
        .value(100)
        .size(246, 23)
        .pos(-1, 80)
        .bg_color(lv::rgb(0x2F1C25))
        .bg_opa(lv::opa::cover)
        .indicator_bg_color(lv::rgb(0x804E66))
        .indicator_bg_opa(lv::opa::cover)
        .indicator_bg_grad_color(lv::rgb(0xF697C4))
        .indicator_bg_grad_dir(lv::kGradDir::hor)
        .get();

    // Distance bar
    m_bar_distance = lv::Bar::create(m_screen)
        .value(100)
        .size(246, 23)
        .pos(0, 103)
        .bg_color(lv::rgb(0x0E301A))
        .bg_opa(lv::opa::cover)
        .indicator_bg_color(lv::rgb(0x268047))
        .indicator_bg_opa(lv::opa::cover)
        .indicator_bg_grad_color(lv::rgb(0x4CFE8D))
        .indicator_bg_grad_dir(lv::kGradDir::hor)
        .get();

    // Calories bar
    m_bar_calories = lv::Bar::create(m_screen)
        .value(100)
        .size(246, 23)
        .pos(0, 126)
        .bg_color(lv::rgb(0x112E2E))
        .bg_opa(lv::opa::cover)
        .indicator_bg_color(lv::rgb(0x30807F))
        .indicator_bg_opa(lv::opa::cover)
        .indicator_bg_grad_color(lv::rgb(0x5CF2F1))
        .indicator_bg_grad_dir(lv::kGradDir::hor)
        .get();

    // Mask gradient image
    lv::Image::create(m_screen).src(&image_mask_gradient);

    // Time label
    m_label_time = lv::Label::create(m_screen)
        .pos(3, 25)
        .align(lv::kAlign::top_mid)
        .text("13:37")
        .text_font(&font_inter_bold_28)
        .get();

    // Steps label
    m_label_steps = lv::Label::create(m_screen)
        .pos(-180, 157)
        .align(lv::kAlign::top_right)
        .text("10,230")
        .text_color(lv::rgb(0xF697C4))
        .text_font(&font_inter_bold_57)
        .get();

    // Distance label
    m_label_distance = lv::Label::create(m_screen)
        .pos(-181, 212)
        .align(lv::kAlign::top_right)
        .text("383")
        .text_color(lv::rgb(0x4CFE8D))
        .text_font(&font_inter_bold_57)
        .get();

    // Calories label
    m_label_calories = lv::Label::create(m_screen)
        .pos(-180, 272)
        .align(lv::kAlign::top_right)
        .text("24")
        .text_color(lv::rgb(0x5CF2F1))
        .text_font(&font_inter_bold_57)
        .get();

    // Animation using this pointer as user_data
    lv::Anim()
        .var(this)
        .values(50, 200)
        .duration(2000)
        .exec([](void* var, int32_t v) {
            static_cast<SportsScreen*>(var)->animate_arcs(v);
        })
        .playback_duration(2000)
        .repeat_infinite()
        .start();
}

inline void SportsScreen::animate_arcs(int32_t v) {
    int bar_value = v > 100 ? 100 : v;
    int arc_value = v > 100 ? v - 100 : 0;

    lv::Bar(lv::wrap, m_bar_steps).value(bar_value);
    lv::Bar(lv::wrap, m_bar_distance).value(bar_value);
    lv::Bar(lv::wrap, m_bar_calories).value(bar_value);

    lv::Arc(lv::wrap, m_arc_steps).value(arc_value / 5 * 4);
    lv::Arc(lv::wrap, m_arc_distance).value(arc_value / 2);
    lv::Arc(lv::wrap, m_arc_calories).value(arc_value / 3 * 2);
}

// on_gesture implementation deferred to smartwatch_gestures.hpp
// to avoid incomplete type issues with forward-declared screen classes

} // namespace smartwatch

#pragma once
/**
 * @file smartwatch_weather.hpp
 * @brief Weather screen for smartwatch demo
 */

#include "smartwatch_private.hpp"

// Font declarations (must be outside namespace - C symbols)
LV_FONT_DECLARE(font_inter_regular_28);
LV_FONT_DECLARE(font_inter_bold_32);
LV_FONT_DECLARE(font_inter_regular_24);
LV_FONT_DECLARE(font_inter_regular_48);
LV_FONT_DECLARE(font_inter_light_32);
LV_FONT_DECLARE(font_inter_regular_70);
LV_FONT_DECLARE(font_inter_light_124);
LV_FONT_DECLARE(font_inter_light_24);

// Image declarations (must be outside namespace - C symbols)
LV_IMAGE_DECLARE(image_weather_bg);
LV_IMAGE_DECLARE(image_pressure_icon);
LV_IMAGE_DECLARE(image_pressure_scale);
LV_IMAGE_DECLARE(image_uv_icon);
LV_IMAGE_DECLARE(image_uv_scale);
LV_IMAGE_DECLARE(image_humidity_icon);
LV_IMAGE_DECLARE(image_sunshine_icon);

// Lottie data declarations (C symbols)
extern "C" {
    extern uint8_t lottie_sun_cloud[];
    extern size_t lottie_sun_cloud_size;
}

namespace smartwatch {

class WeatherScreen {
public:
    void create(DemoController& controller);
    [[nodiscard]] lv_obj_t* get() const { return m_screen; }

private:
    void on_gesture(lv::Event e);

    lv_obj_t* create_info_card(lv_obj_t* parent, const char* title, const void* icon);
    lv_obj_t* create_forecast_container(lv_obj_t* parent, const char* title);
    void add_hourly_forecast(lv_obj_t* parent, int32_t hour, const void* icon, int32_t temp);
    void add_weekly_forecast(lv_obj_t* parent, const char* day, const void* icon, int32_t low, int32_t high);

    lv_obj_t* m_screen = nullptr;
    DemoController* m_controller = nullptr;
    lv::Style m_main_style;
    bool m_inited = false;
};

inline lv_obj_t* WeatherScreen::create_info_card(lv_obj_t* parent, const char* title, const void* icon) {
    auto card = lv::Box::create(parent);
    card.remove_all_styles()
        .size(200, 214)
        .align(lv::kAlign::top_mid)
        .bg_color(lv::rgb(0x000000))
        .bg_opa(50)
        .radius(12)
        .pad_top(16)
        .pad_bottom(24)
        .pad_left(20)
        .pad_right(20)
        .remove_flag(lv::kFlag::scrollable);

    auto cont = lv::Box::create(card);
    cont.remove_all_styles()
        .size(156, lv::kSize::content);

    lv::Image::create(cont)
        .src(icon)
        .size(lv::kSize::content, lv::kSize::content);

    lv::Label::create(cont)
        .pos(35, 0)
        .align(lv::kAlign::bottom_left)
        .text(title)
        .text_font(&font_inter_regular_24);

    return card.get();
}

inline lv_obj_t* WeatherScreen::create_forecast_container(lv_obj_t* parent, const char* title) {
    auto main_cont = lv::vbox(lv::ObjectView(parent))
        .fill_width()
        .height(lv::kSize::content)
        .align(lv::kFlexAlign::start, lv::kFlexAlign::center, lv::kFlexAlign::center)
        .pad_top(40)
        .row_gap(24);

    lv::Label::create(main_cont)
        .text(title)
        .text_font(&font_inter_regular_28);

    auto forecast_cont = lv::vbox(main_cont)
        .fill_width()
        .height(lv::kSize::content)
        .align(lv::kFlexAlign::start, lv::kFlexAlign::center, lv::kFlexAlign::center)
        .pad_top(24)
        .pad_bottom(24)
        .row_gap(26);

    return forecast_cont.get();
}

inline void WeatherScreen::add_hourly_forecast(lv_obj_t* parent, int32_t hour, const void* icon, int32_t temp) {
    auto main_cont = lv::hbox(lv::ObjectView(parent))
        .size(336, lv::kSize::content)
        .space_evenly()
        .align_items(lv::kFlexAlign::center);

    lv::Label::create(main_cont)
        .text_fmt("%02d", (int)hour)
        .text_font(&font_inter_regular_28);

    lv::Image::create(main_cont).src(icon);

    lv::Label::create(main_cont)
        .text_fmt("%02d°", (int)temp)
        .text_font(&font_inter_regular_28);
}

inline void WeatherScreen::add_weekly_forecast(lv_obj_t* parent, const char* day, const void* icon, int32_t low, int32_t high) {
    auto main_cont = lv::hbox(lv::ObjectView(parent))
        .size(336, lv::kSize::content)
        .space_evenly()
        .align_items(lv::kFlexAlign::center);

    lv::Label::create(main_cont)
        .text(day)
        .text_font(&font_inter_regular_28);

    lv::Image::create(main_cont).src(icon);

    lv::Label::create(main_cont)
        .text_fmt("%02d° / %02d°", (int)low, (int)high)
        .text_font(&font_inter_regular_28);
}

inline void WeatherScreen::create(DemoController& controller) {
    m_controller = &controller;

    if (!m_inited) {
        m_main_style.text_color(lv::colors::white())
            .text_opa(lv::opa::_100)
            .bg_color(lv::rgb(0x316bb6))
            .bg_opa(lv::opa::_100)
            .translate_x(SCREEN_SIZE);
        m_inited = true;
    }

    auto screen = lv::Box::create(lv::screen_active());
    screen.remove_all_styles()
        .add_style(m_main_style.get())
        .fill()
        .scrollbar_mode(lv::kScrollbarMode::off)
        .scroll_dir(lv::kDir::ver)
        .remove_flag(lv::kFlag::gesture_bubble)
        .remove_flag(lv::kFlag::event_bubble);
    screen.on<&WeatherScreen::on_gesture>(lv::kEvent::gesture, this);
    m_screen = screen.get();

    // Background
    lv::Image::create(m_screen)
        .pos(0, 0)
        .align(lv::kAlign::top_mid)
        .size(lv::kSize::content, lv::kSize::content)
        .src(&image_weather_bg);

    // Time label
    lv::Label::create(m_screen)
        .text("13:37")
        .align(lv::kAlign::top_mid)
        .y(30)
        .text_font(&font_inter_regular_28);

    // Main container
    auto main_cont = lv::vbox(lv::ObjectView(m_screen))
        .fill_width()
        .height(lv::kSize::content)
        .scroll_dir(lv::kDir::ver)
        .align(lv::kFlexAlign::start, lv::kFlexAlign::center, lv::kFlexAlign::center)
        .pad_top(40)
        .pad_bottom(64)
        .row_gap(20);

    // Weather details container
    auto cont = lv::vbox(main_cont)
        .size(lv::pct(100), 335)
        .center_content();

    lv::Label::create(cont)
        .text("Budapest")
        .text_font(&font_inter_regular_28);

    // Temperature container
    auto temp_cont = lv::hbox(cont)
        .size(lv::kSize::content, lv::kSize::content)
        .column_gap(5)
        .center_content();

#if LV_USE_LOTTIE == 1
    auto sun_icon = lv::Lottie::create(temp_cont)
        .src_data(lottie_sun_cloud, lottie_sun_cloud_size);
    sun_icon.size(lv::kSize::content, lv::kSize::content)
        .align(lv::kAlign::center)
        .add_flag(lv::kFlag::clickable)
        .remove_flag(lv::kFlag::scrollable);
#if LV_DRAW_BUF_ALIGN == 4 && LV_DRAW_BUF_STRIDE_ALIGN == 1
    static uint8_t sun_buf[100 * 89 * 4];
    sun_icon.buffer(100, 89, sun_buf);
#else
    LV_DRAW_BUF_DEFINE(sun_buf, 64, 64, LV_COLOR_FORMAT_ARGB8888);
    sun_icon.draw_buf(&sun_buf);
#endif
#endif

    lv::Label::create(temp_cont)
        .text("12°")
        .text_font(&font_inter_light_124);

    // Min/Max container
    auto text_cont = lv::hbox(cont)
        .size(334, 50)
        .column_gap(5)
        .center_content();

    lv::Label::create(text_cont)
        .text("Max. 28°")
        .text_font(&font_inter_regular_28);

    lv::Label::create(text_cont)
        .text("Min. 07°")
        .text_color(lv::rgb(0x94d4fd))
        .text_font(&font_inter_regular_28);

    // Info cards container
    auto cont_cards = lv::vbox(main_cont)
        .size(385, lv::kSize::content)
        .center_content()
        .row_gap(22);

    // Pressure card
    lv_obj_t* card = create_info_card(cont_cards, "PRESSURE", &image_pressure_icon);

    lv::Image::create(card)
        .src(&image_pressure_scale)
        .align(lv::kAlign::center)
        .y(5);

    lv::Label::create(card)
        .text("1013")
        .y(5)
        .align(lv::kAlign::center)
        .text_font(&font_inter_bold_32);

    lv::Label::create(card)
        .pos(0, 35)
        .text("hPa")
        .align(lv::kAlign::center)
        .text_font(&font_inter_regular_24);

    lv::Label::create(card)
        .text("Low")
        .align(lv::kAlign::bottom_left)
        .text_font(&font_inter_regular_24);

    lv::Label::create(card)
        .text("High")
        .align(lv::kAlign::bottom_right)
        .text_font(&font_inter_regular_24);

    // UV card
    card = create_info_card(cont_cards, "UV INDEX", &image_uv_icon);
    lv::Slider::create(card)
        .pos(0, -59)
        .size(160, 6)
        .value(25)
        .align(lv::kAlign::bottom_left)
        .bg_image_src(&image_uv_scale)
        .indicator_bg_opa(0)
        .knob_border_opa(lv::opa::cover)
        .knob_border_width(3)
        .knob_border_color(lv::rgb(0x114370))
        .knob_bg_color(lv::rgb(0xffffff))
        .knob_bg_opa(lv::opa::cover);

    lv::Label::create(card)
        .pos(0, 5)
        .text("2")
        .align(lv::kAlign::bottom_left)
        .text_font(&font_inter_regular_48);

    lv::Label::create(card)
        .text("Low")
        .align(lv::kAlign::bottom_mid)
        .text_font(&font_inter_light_32);

    // Humidity card
    card = create_info_card(cont_cards, "HUMIDITY", &image_humidity_icon);
    lv::Label::create(card)
        .text("75%")
        .align(lv::kAlign::bottom_left)
        .text_color(lv::rgb(0xffffff))
        .text_font(&font_inter_regular_70);

    // Today's forecast
    lv_obj_t* panel = create_forecast_container(main_cont, "Today's Forecast");
    for (int32_t i = 0; i < 10; i++) {
        add_hourly_forecast(panel, i, &image_sunshine_icon, 20 + i);
    }

    // 10 days forecast
    panel = create_forecast_container(main_cont, "10 Days Forecast");
    for (int32_t i = 0; i < 10; i++) {
        add_weekly_forecast(panel, "Mon", &image_sunshine_icon, 5 + i, 17 + i);
    }

    // Update time
    lv::Label::create(main_cont)
        .text("Updated Today\nat 12 pm")
        .text_font(&font_inter_light_24)
        .text_align(lv::kTextAlign::center);
}

// on_gesture implementation deferred to smartwatch_gestures.hpp
// to avoid incomplete type issues with forward-declared screen classes

} // namespace smartwatch

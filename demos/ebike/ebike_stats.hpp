#pragma once

/**
 * @file ebike_stats.hpp
 * @brief Statistics screen with chart - C++ port
 */

#include <lv/lv.hpp>
#include "ebike_private.hpp"
#include "translations/lv_i18n.h"
#include <optional>

namespace ebike {

/**
 * @brief Statistics page with tabs, chart, and stat cards
 *
 * Uses optional<IntState> for lazy initialization with RAII cleanup.
 */
class StatsPage {
    lv::Box m_root;

    // Static state using C++ wrapper - persist across page switches
    static inline std::optional<lv::IntState> s_mode;
    static inline std::optional<lv::IntState> s_week;
    static inline std::optional<lv::IntState> s_day;
    static inline std::optional<lv::IntState> s_avg_speed;
    static inline std::optional<lv::IntState> s_distance;
    static inline std::optional<lv::IntState> s_top_speed;

    static inline lv_obj_t* s_left_arrow = nullptr;
    static inline lv_obj_t* s_right_arrow = nullptr;

    static inline int32_t s_top_speed_values[30] = {46, 28, 42, 39, 41, 25, 49, 37, 35, 40, 33, 40, 31, 27, 45, 38, 41, 40, 27, 25, 30, 45, 31, 43, 41, 34, 47, 32, 30, 33};
    static inline int32_t s_avg_speed_values[30] = {21, 24, 27, 29, 23, 28, 28, 22, 29, 28, 24, 26, 24, 30, 25, 25, 20, 28, 24, 27, 25, 27, 20, 29, 25, 24, 23, 26, 27, 27};
    static inline int32_t s_distance_values[30] =  {87, 63, 29, 84, 27, 84, 33, 76, 77, 49, 46, 29, 67, 21, 87, 75, 40, 19, 12, 67, 66, 11, 59, 33, 51, 75, 44, 61, 53, 63};

    enum Mode {
        MODE_AVG_SPEED = 0,
        MODE_DISTANCE = 1,
        MODE_TOP_SPEED = 2
    };

public:
    void init() {
        if (s_mode.has_value()) return;  // Already initialized

        s_mode.emplace(MODE_DISTANCE);
        s_week.emplace(0);
        s_day.emplace(0);
        s_avg_speed.emplace(0);
        s_distance.emplace(0);
        s_top_speed.emplace(0);
    }

    void deinit() {
        s_mode.reset();
        s_week.reset();
        s_day.reset();
        s_avg_speed.reset();
        s_distance.reset();
        s_top_speed.reset();
    }

    void create(lv::ObjectView parent) {
        m_root = lv::Box::create(parent)
            .fill()
            .bg_opa(lv::opa::transp)
            .flex_flow(lv::kFlexFlow::row);

        create_left_cont();
        create_right_cont();
    }

    [[nodiscard]] lv::Box& root() { return m_root; }

private:
    void create_left_cont() {
        auto left_cont = lv::Box::create(m_root)
            .size(164, lv::pct(100))
            .bg_opa(lv::opa::transp)
            .remove_flag(lv::kFlag::scrollable);

        lv::Label::create(left_cont)
            .text(_("STATS"))
            .text_font(font_medium())
            .align(lv::kAlign::top_left, 24, 16);

        lv::Image::create(left_cont)
            .src(&img_ebike_stats_large)
            .align(lv::kAlign::bottom_mid);
    }

    void create_right_cont() {
        auto right_cont = lv::Box::create(m_root)
            .fill()
            .bg_opa(lv::opa::transp)
            .grow()
            .padding_ver(12)
            .pad_right(8)
            .pad_row(8)
            .fill_height()
            .flex_flow(lv::kFlexFlow::column)
            .flex_main_place(lv::kFlexAlign::space_between);

        create_tabs(right_cont);
        create_data_cont(right_cont);
        create_chart(right_cont);
        create_stat_cont(right_cont);
    }

    void create_tabs(lv::ObjectView parent) {
        auto btnm = lv::ButtonMatrix::create(parent)
            .size(lv::pct(100), 24)
            .bg_opa(lv::opa::_0)
            .bg_opa(lv::opa::_0, lv::kPart::items)
            .border_width(1, lv::kPart::items)
            .border_color(turquoise(), lv::kPart::items)
            .text_font(font_small(), lv::kPart::items)
            .text_color(lv::colors::white(), lv::kPart::items)
            .border_side(lv::kBorderSide::bottom, lv::kPart::items)
            .border_opa(lv::opa::_20, lv::kPart::items);

        // Need to use translated strings - store in static array that persists
        static const char* texts[4];
        texts[0] = _("Avg. speed");
        texts[1] = _("Distance");
        texts[2] = _("Top speed");
        texts[3] = nullptr;
        btnm.map(texts)
            .selected(s_mode->get())
            .on_value_changed([](lv::Event e) {
                lv_obj_t* btnm_obj = e.target();
                s_mode->set(lv::ButtonMatrix(lv::wrap, btnm_obj).selected());
            });
    }

    void create_data_cont(lv::ObjectView parent) {
        // Reset stale pointers from previous page creation — the old widgets
        // were deleted on page switch, so these would dangle.
        s_left_arrow = nullptr;
        s_right_arrow = nullptr;

        auto cont = lv::Box::create(parent)
            .size(lv::pct(100), lv::kSize::content)
            .bg_opa(lv::opa::transp)
            .text_color(lv::colors::white())
            .flex_flow(lv::kFlexFlow::column)
            .flex_align(lv::kFlexAlign::center, lv::kFlexAlign::center, lv::kFlexAlign::space_between);

        // Left arrow
        auto left_arrow = lv::Image::create(cont)
            .src(&img_ebike_arrow_left_2)
            .size(40, 40)
            .clickable()
            .ext_click_area(32)
            .on_click([](lv::Event) {
                if (s_week->get() > 0) {
                    s_week->decrement();
                }
            });
        s_left_arrow = left_arrow.get();

        // Data label
        auto data_label = lv::Label::create(cont)
            .text("0km")
            .text_font(font_large())
            .add_flag(lv::kFlag::flex_in_new_track);

        // Observer for data label - uses static callback
        s_mode->observe_obj(data_observer_cb, data_label);
        s_day->observe_obj(data_observer_cb, data_label);

        // Week label
        auto week_label = lv::Label::create(cont)
            .text("March 1 - March 7")
            .text_font(font_small());

        // Observer for week label
        s_week->observe_obj(week_observer_cb, week_label);

        // Right arrow
        auto right_arrow = lv::Image::create(cont)
            .src(&img_ebike_arrow_right_2)
            .size(40, 40)
            .clickable()
            .add_flag(lv::kFlag::flex_in_new_track)
            .ext_click_area(32)
            .on_click([](lv::Event) {
                if (s_week->get() < 3) {
                    s_week->increment();
                }
            });
        s_right_arrow = right_arrow.get();
    }

    static void data_observer_cb(lv_observer_t* observer, lv_subject_t*) {
        lv::Label label(lv::wrap, lv::observer_get_target_obj(observer));
        int32_t mode = s_mode->get();
        int32_t day = s_day->get();
        switch (mode) {
            case MODE_AVG_SPEED:
                label.text_fmt("%dkm/h", s_avg_speed_values[day]);
                break;
            case MODE_TOP_SPEED:
                label.text_fmt("%dkm/h", s_top_speed_values[day]);
                break;
            case MODE_DISTANCE:
                label.text_fmt("%dkm", s_distance_values[day]);
                break;
        }
    }

    static void week_observer_cb(lv_observer_t* observer, lv_subject_t*) {
        lv::Label label(lv::wrap, lv::observer_get_target_obj(observer));
        int32_t week = s_week->get();
        label.text_fmt("March %d - March %d", week * 7 + 1, week * 7 + 7);

        // Update arrow opacity
        if (s_left_arrow) {
            lv::Image(lv::wrap, s_left_arrow).image_opa(week == 0 ? lv::opa::_50 : lv::opa::_100);
        }
        if (s_right_arrow) {
            lv::Image(lv::wrap, s_right_arrow).image_opa(week == 3 ? lv::opa::_50 : lv::opa::_100);
        }
    }

    void create_chart(lv::ObjectView parent) {
        auto cont = lv::Box::create(parent)
            .fill_width()
            .bg_opa(lv::opa::transp)
            .grow()
            .remove_flag(lv::kFlag::scrollable);

        auto chart = lv::Chart::create(cont)
            .fill()
            .update_circular()
            .point_count(30)
            .grow()
            .bg_opa(lv::opa::_0)
            .border_width(1)
            .border_color(lv::rgb(0x333333))
            .line_width(3, lv::kPart::items)
            .line_color(turquoise(), lv::kPart::items)
            .part_size(8, 8, lv::kPart::indicator)
            .bg_opa(lv::opa::cover, lv::kPart::indicator)
            .bg_color(turquoise(), lv::kPart::indicator)
            .radius(lv::kRadius::circle, lv::kPart::indicator)
            .border_side(lv::kBorderSide::bottom);

        chart.div_lines(0, 0);

        // Add series with initial data
        auto* ser = chart.add_series(turquoise());
        chart.set_ext_y_array(ser, s_distance_values)
             .range_y(0, 90);

        // Observer for mode change
        s_mode->observe_obj(chart_mode_observer_cb, chart);
    }

    static void chart_mode_observer_cb(lv_observer_t* observer, lv_subject_t*) {
        lv::Chart chart(lv::wrap, lv::observer_get_target_obj(observer));
        auto* ser = chart.get_first_series();
        int32_t mode = s_mode->get();

        switch (mode) {
            case MODE_AVG_SPEED:
                chart.range_y(0, 30).set_ext_y_array(ser, s_avg_speed_values);
                break;
            case MODE_TOP_SPEED:
                chart.range_y(0, 50).set_ext_y_array(ser, s_top_speed_values);
                break;
            case MODE_DISTANCE:
                chart.range_y(0, 90).set_ext_y_array(ser, s_distance_values);
                break;
        }
        chart.refresh();
    }

    void create_stat_cont(lv::ObjectView parent) {
        auto cont = lv::Box::create(parent)
            .size(lv::pct(100), lv::kSize::content)
            .bg_opa(lv::opa::transp)
            .flex_flow(lv::kFlexFlow::row);

        create_stat_card(cont, _("Avg. speed"), *s_avg_speed, "%dkm/h");
        create_stat_card(cont, _("Distance"), *s_distance, "%dkm");
        create_stat_card(cont, _("Top speed"), *s_top_speed, "%dkm/h");
    }

    void create_stat_card(lv::ObjectView parent, const char* name, lv::IntState& state, const char* fmt) {
        auto cont = lv::Box::create(parent)
            .height(lv::kSize::content)
            .bg_opa(lv::opa::transp)
            .grow()
            .flex_flow(lv::kFlexFlow::column)
            .flex_align(lv::kFlexAlign::start, lv::kFlexAlign::center, lv::kFlexAlign::center);

        lv::Label::create(cont)
            .text(name)
            .text_font(font_small());

        lv::Label::create(cont)
            .text("0")
            .text_font(font_medium())
            .bind_text(state.subject(), fmt);
    }
};

} // namespace ebike

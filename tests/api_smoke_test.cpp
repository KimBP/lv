/**
 * @file api_smoke_test.cpp
 * @brief Compile-only API smoke test
 *
 * This file does NOT run — it only needs to compile.
 * It catches overload ambiguities, missing methods, and name
 * collisions across the ObjectView / ObjectMixin / widget hierarchy.
 *
 * Build with: g++ -std=c++20 -fsyntax-only -I include -I ../lvgl -I . tests/api_smoke_test.cpp
 */

#include <lv/lv.hpp>
#include <lv/core/verify.hpp>

// ============================================================
// user_data: no ambiguity on widgets or raw ObjectView
// ============================================================

[[maybe_unused]] static void test_user_data() {
    lv::Button btn;
    void* ptr = nullptr;

    // Widget setter (returns Button&, from ObjectMixin)
    btn.user_data(ptr);

    // Widget getter (from ObjectMixin)
    void* a = btn.user_data();

    // Widget typed getter (from ObjectMixin)
    int* b = btn.user_data_as<int>();

    // Raw ObjectView getter (get_ prefix to avoid ambiguity with ObjectMixin)
    lv::ObjectView obj(nullptr);
    void* c = obj.get_user_data();
    int* d = obj.get_user_data<int>();

    (void)a; (void)b; (void)c; (void)d;
}

// ============================================================
// user_data on Grid (regression: was ambiguous after adding ObjectMixin)
// ============================================================

[[maybe_unused]] static void test_grid_user_data() {
    lv::Grid grid(lv::ObjectView(nullptr));
    void* ptr = nullptr;

    grid.user_data(ptr);
    void* a = grid.user_data();
    int* b = grid.user_data_as<int>();

    (void)a; (void)b;
}

// ============================================================
// Inherited ObjectMixin methods on Box, Textarea, Slider, Grid
// (regression: were duplicated, now inherited)
// ============================================================

[[maybe_unused]] static void test_inherited_methods() {
    lv::Box box;
    box.size(100, 50);
    box.width(100);
    box.height(50);
    box.fill();
    box.fill_width();
    box.fill_height();
    box.pos(0, 0);
    box.x(0);
    box.y(0);
    box.center();
    box.grow();
    box.scrollable(false);

    lv::Textarea ta;
    ta.size(200, 100);
    ta.width(200);
    ta.height(100);
    ta.fill_width();

    lv::Slider sl;
    sl.width(200);
    sl.fill_width();

    lv::Grid grid(lv::ObjectView(nullptr));
    grid.size(300, 200);
    grid.width(300);
    grid.height(200);
    grid.size_content();
    grid.fill();
    grid.fill_width();
    grid.fill_height();
}

// ============================================================
// Fluent chaining returns correct type
// ============================================================

[[maybe_unused]] static void test_fluent_chaining() {
    lv::Button btn;

    // Each method returns Button&, allowing further Button-specific calls
    btn.size(100, 50).text("OK").on_click([](lv::Event) {});

    lv::Box box;
    box.fill().bg_color(lv::rgb(0xFF0000)).padding(10);
}

// ============================================================
// Grid uses global lv::wrap_t (not its own)
// ============================================================

[[maybe_unused]] static void test_grid_wrap() {
    lv_obj_t* raw = nullptr;
    lv::Grid grid(lv::wrap, raw);
    (void)grid;
}

// ============================================================
// Event API overloads
// ============================================================

struct Dummy {
    void on_click(lv::Event) {}
    void on_value(lv_event_t*) {}
    void on_simple() {}
};

[[maybe_unused]] static void test_event_overloads() {
    lv::Button btn;
    Dummy d;

    // Stateless lambda (lv_event_t*)
    btn.on(LV_EVENT_CLICKED, [](lv_event_t*) {});

    // Stateless lambda (lv::Event)
    btn.on(LV_EVENT_CLICKED, [](lv::Event) {});

    // Member function (lv::Event)
    btn.on<&Dummy::on_click>(LV_EVENT_CLICKED, &d);

    // Member function (lv_event_t*)
    btn.on<&Dummy::on_value>(LV_EVENT_CLICKED, &d);

    // Member function (void())
    btn.on<&Dummy::on_simple>(LV_EVENT_CLICKED, &d);

    // Convenience shorthands
    btn.on_click([](lv::Event) {});
    btn.on_click<&Dummy::on_click>(&d);
}

// ============================================================
// ObjectView getters: parent().get_width(), child().get_height()
// (regression: getters were only on ObjectMixin, not ObjectView)
// ============================================================

[[maybe_unused]] static void test_objectview_getters() {
    lv::Button btn;

    // Geometry getters on ObjectView returned by parent()/child()
    int32_t w = btn.parent().get_width();
    int32_t h = btn.parent().get_height();
    int32_t cw = btn.child(0).content_width();
    int32_t ch = btn.child(0).content_height();

    // Scroll getters
    int32_t sx = btn.parent().scroll_x();
    int32_t sy = btn.parent().scroll_y();

    // Extended draw size
    int32_t eds = btn.parent().calculate_ext_draw_size();

    // user_data on bare ObjectView
    void* ud = btn.parent().get_user_data();

    (void)w; (void)h; (void)cw; (void)ch;
    (void)sx; (void)sy; (void)eds; (void)ud;
}

int main() {
    return 0;
}

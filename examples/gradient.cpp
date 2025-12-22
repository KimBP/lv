/**
 * @file gradient.cpp
 * @brief Gradient example demonstrating LVGL gradients
 *
 * LVGL supports two gradient approaches:
 *
 * 1. SIMPLE GRADIENTS (2 colors, horizontal or vertical only)
 *    - Use: bg_color() + bg_grad_color() + bg_grad_dir()
 *    - No special config needed
 *    - Works everywhere
 *
 * 2. COMPLEX GRADIENTS (angles, radial, conical, multi-stop)
 *    - Use: lv::GradDsc with bg_grad()
 *    - Requires: LV_USE_DRAW_SW_COMPLEX_GRADIENTS=1 in lv_conf.h
 *    - Requires: LV_GRADIENT_MAX_STOPS >= number of color stops
 *    - Requires: bg_opa(LV_OPA_COVER) on the widget
 *    - IMPORTANT: GradDsc must be 'static' (LVGL stores pointer, not copy!)
 */

#include <lv/lv.hpp>
#include <lv/assets/cursor.hpp>

static void create_gradient_boxes(lv::ObjectView parent) {
    auto content = lv::vbox(parent)
        .fill()
        .padding(16)
        .gap(12);

    // Title
    lv::Label::create(content)
        .text("Gradient Examples")
        .font(lv::fonts::montserrat_20)
        .text_color(lv::rgb(0x333333));

    // Row 1: Simple gradients (using bg_color + bg_grad_color + bg_grad_dir)
    lv::Label::create(content)
        .text("Simple gradients:")
        .text_color(lv::rgb(0x666666));

    auto row1 = lv::hbox(content)
        .fill_width()
        .gap(12);

    // Horizontal gradient - simple method
    auto box1 = lv::Box::create(row1)
        .size(100, 80)
        .radius(8)
        .bg_color(lv::rgb(0xFF6B6B))
        .bg_grad_color(lv::rgb(0x4ECDC4))
        .bg_grad_dir(LV_GRAD_DIR_HOR);

    lv::Label::create(box1)
        .text("Horizontal")
        .text_color(lv::colors::white())
        .center();

    // Vertical gradient - simple method
    auto box2 = lv::Box::create(row1)
        .size(100, 80)
        .radius(8)
        .bg_color(lv::rgb(0xA8E6CF))
        .bg_grad_color(lv::rgb(0x3D5A80))
        .bg_grad_dir(LV_GRAD_DIR_VER);

    lv::Label::create(box2)
        .text("Vertical")
        .text_color(lv::colors::white())
        .center();

    // Another vertical with different colors
    auto box3 = lv::Box::create(row1)
        .size(100, 80)
        .radius(8)
        .bg_color(lv::rgb(0x667eea))
        .bg_grad_color(lv::rgb(0x764ba2))
        .bg_grad_dir(LV_GRAD_DIR_VER);

    lv::Label::create(box3)
        .text("Purple")
        .text_color(lv::colors::white())
        .center();

    // Row 2: Complex gradients (requires LV_USE_DRAW_SW_COMPLEX_GRADIENTS)
    lv::Label::create(content)
        .text("Complex gradients:")
        .text_color(lv::rgb(0x666666));

    auto row2 = lv::hbox(content)
        .fill_width()
        .gap(12);

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS
    // Linear gradient at an angle (diagonal)
    static auto linear_grad = lv::GradDsc()
        .colors(lv::rgb(0xFFE66D), lv::rgb(0xFF6B6B))
        .linear(lv_pct(0), lv_pct(0), lv_pct(100), lv_pct(100));

    auto box4 = lv::Box::create(row2)
        .size(100, 80)
        .radius(8)
        .bg_opa(LV_OPA_COVER)
        .bg_grad(linear_grad.get());

    lv::Label::create(box4)
        .text("Diagonal")
        .text_color(lv::colors::white())
        .center();

    // Radial gradient
    static auto radial_grad = lv::GradDsc()
        .colors(lv::rgb(0xFFFFFF), lv::rgb(0x2C3E50))
        .radial(lv_pct(30), lv_pct(30), lv_pct(100), lv_pct(100));

    auto box5 = lv::Box::create(row2)
        .size(100, 80)
        .radius(8)
        .bg_opa(LV_OPA_COVER)
        .bg_grad(radial_grad.get());

    lv::Label::create(box5)
        .text("Radial")
        .text_color(lv::rgb(0x333333))
        .center();

    // Conical gradient (rainbow wheel)
    static auto conical_grad = lv::GradDsc()
        .stop(lv::rgb(0xE74C3C), 0)      // Red
        .stop(lv::rgb(0xF39C12), 85)     // Orange
        .stop(lv::rgb(0x2ECC71), 170)    // Green
        .stop(lv::rgb(0xE74C3C), 255)    // Back to red
        .conical(lv_pct(50), lv_pct(50), 0, 360);

    auto box6 = lv::Box::create(row2)
        .size(100, 80)
        .radius(8)
        .bg_opa(LV_OPA_COVER)
        .bg_grad(conical_grad.get());

    lv::Label::create(box6)
        .text("Conical")
        .text_color(lv::colors::white())
        .center();

    // Row 3: More complex gradients
    auto row3 = lv::hbox(content)
        .fill_width()
        .gap(12);

    // Rainbow gradient (multi-stop horizontal)
    static auto rainbow_grad = lv::GradDsc()
        .stop(lv::rgb(0xFF0000), 0)     // Red
        .stop(lv::rgb(0xFFFF00), 64)    // Yellow
        .stop(lv::rgb(0x00FF00), 128)   // Green
        .stop(lv::rgb(0x0000FF), 192)   // Blue
        .stop(lv::rgb(0xFF00FF), 255)   // Magenta
        .linear(lv_pct(0), lv_pct(50), lv_pct(100), lv_pct(50));  // Horizontal

    auto box7 = lv::Box::create(row3)
        .size(150, 80)
        .radius(8)
        .bg_opa(LV_OPA_COVER)
        .bg_grad(rainbow_grad.get());

    lv::Label::create(box7)
        .text("Rainbow")
        .text_color(lv::colors::white())
        .center();

    // Reflected linear gradient (striped effect)
    static auto reflect_grad = lv::GradDsc()
        .colors(lv::rgb(0x26a0da), lv::rgb(0x314755))
        .linear(lv_pct(0), lv_pct(0), lv_pct(30), lv_pct(100), LV_GRAD_EXTEND_REFLECT);

    auto box8 = lv::Box::create(row3)
        .size(100, 80)
        .radius(8)
        .bg_opa(LV_OPA_COVER)
        .bg_grad(reflect_grad.get());

    lv::Label::create(box8)
        .text("Reflect")
        .text_color(lv::colors::white())
        .center();

#else
    lv::Label::create(row2)
        .text("Enable LV_USE_DRAW_SW_COMPLEX_GRADIENTS\nfor linear/radial/conical gradients")
        .text_color(lv::rgb(0x888888));
#endif

    // Info
    lv::Label::create(content)
        .text("Simple: bg_color + bg_grad_color + bg_grad_dir\n"
              "Complex: lv_grad_init_stops + lv_grad_*_init")
        .text_color(lv::rgb(0x888888));
}

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("Gradient Demo", 420, 480, &lv::cursor_arrow);
#elif LV_USE_SDL
    lv::SDLDisplay display(420, 480);
#else
    #error "No display backend enabled"
#endif

    create_gradient_boxes(lv::screen_active());

    lv::run();
    return 0;
}

/**
 * @file hello.cpp
 * @brief Minimal "Hello World" example for lv:: C++ bindings
 *
 * The simplest possible example - just a label on screen.
 *
 * Build:
 *   cmake -B build -DLVGL_DIR=../lvgl
 *   cmake --build build
 *   ./build/examples/hello
 */

#include <lv/lv.hpp>

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("Hello LVGL", 480, 320);
#elif LV_USE_SDL
    lv::SDLDisplay display(480, 320);
#else
    #error "No display backend enabled. Enable LV_USE_X11 or LV_USE_SDL in lv_conf.h"
#endif

    lv::Label::create(lv::screen_active())
        .text("Hello, LVGL!")
        .font(lv::fonts::montserrat_28)
        .center();

    lv::run();
}

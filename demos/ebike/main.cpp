/**
 * @file main.cpp
 * @brief E-Bike demo entry point - C++ port
 *
 * This is a C++ port of the official LVGL E-Bike demo using
 * zero-cost C++20 bindings.
 */

#include <lv/lv.hpp>
#include "ebike_demo.hpp"

static ebike::EbikeDemo demo;

int main() {
    lv::init();

    // Create display based on available backend
#if LV_USE_X11
    lv::X11Display display("E-Bike Demo (C++ Port)", 480, 320);
#elif LV_USE_SDL
    lv::SDLDisplay display(480, 320);
#else
    #error "No display backend enabled. Enable LV_USE_X11 or LV_USE_SDL in lv_conf.h"
#endif

    demo.create();

    // Run the main loop
    lv::run();

    demo.destroy();

    return 0;
}

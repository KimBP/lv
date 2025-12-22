/**
 * @file main.cpp
 * @brief Smartwatch demo entry point
 */

#include <lv/lv.hpp>
#include <lv/assets/cursor.hpp>
#include "smartwatch_demo.hpp"
#include "smartwatch_gestures.hpp"

static smartwatch::SmartwatchDemo demo;

int main() {
    lv::init();

    // Create display based on available backend (384x384 for smartwatch)
#if LV_USE_X11
    lv::X11Display display("Smartwatch Demo (C++ Port)", smartwatch::SCREEN_SIZE, smartwatch::SCREEN_SIZE, &lv::cursor_arrow);
#elif LV_USE_SDL
    lv::SDLDisplay display(smartwatch::SCREEN_SIZE, smartwatch::SCREEN_SIZE);
#else
    #error "No display backend enabled. Enable LV_USE_X11 or LV_USE_SDL in lv_conf.h"
#endif

    // Create and run the smartwatch demo
    demo.create();

    // Run the main loop
    lv::run();

    return 0;
}

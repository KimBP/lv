/**
 * @file main.cpp
 * @brief Analog clock demo entry point
 *
 * Displays an analog clock face with rotating hands showing real time.
 */

#include <lv/lv.hpp>
#include "analog_clock.hpp"

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("Analog Clock Demo", 390, 390);
#elif LV_USE_SDL
    lv::SDLDisplay display(390, 390);
#else
#error "No display backend enabled. Enable LV_USE_X11 or LV_USE_SDL in lv_conf.h"
#endif

    analog_clock::AnalogClockDemo demo;
    demo.create();

    lv::run();

    demo.destroy();

    return 0;
}

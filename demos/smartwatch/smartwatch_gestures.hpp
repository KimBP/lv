#pragma once
/**
 * @file smartwatch_gestures.hpp
 * @brief Deferred gesture handlers for smartwatch demo
 *
 * This file contains the on_gesture implementations that were deferred
 * from individual screen headers because they need access to DemoController
 * (which is defined in smartwatch_demo.hpp).
 *
 * Include this AFTER smartwatch_demo.hpp to have full type information.
 */

#include "smartwatch_demo.hpp"

namespace smartwatch {

// ControlScreen::on_gesture
inline void ControlScreen::on_gesture(lv::Event e) {
    (void)e;
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

    if (dir == lv::kDir::left) {
        animate_y(m_screen, -SCREEN_SIZE, 1000, 0);
        animate_arc(m_controller->arc_cont(), ArcAnimation::EXPAND_UP, 700, 300);
        animate_opa(m_controller->main_arc(), 255, 500, 500);
    }
}

inline void ControlScreen::on_long_press(lv::Event e) {
    (void)e;
    animate_y(m_screen, -SCREEN_SIZE, 1000, 0);
    animate_arc(m_controller->arc_cont(), ArcAnimation::EXPAND_UP, 700, 300);
    animate_opa(m_controller->main_arc(), 255, 500, 500);
}

// WeatherScreen::on_gesture
inline void WeatherScreen::on_gesture(lv::Event e) {
    (void)e;
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

    if (dir == lv::kDir::right) {
        animate_x(m_screen, SCREEN_SIZE, 1000, 0);
        animate_arc(m_controller->arc_cont(), ArcAnimation::EXPAND_RIGHT, 700, 300);
        animate_opa(m_controller->main_arc(), 255, 500, 500);
        animate_opa(m_controller->overlay(), 0, 100, 0);
    }
    if (dir == lv::kDir::left) {
        animate_opa(m_controller->overlay(), 255, 100, 0);
        animate_x(m_screen, -SCREEN_SIZE - TRANSITION_GAP, 1000, 0);
        animate_x_from(m_controller->health_screen().get(), SCREEN_SIZE + TRANSITION_GAP, 0, 1000, 0);
    }
}

// HealthScreen::on_gesture
inline void HealthScreen::on_gesture(lv::Event e) {
    (void)e;
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

    if (dir == lv::kDir::left) {
        animate_x(m_screen, -SCREEN_SIZE - TRANSITION_GAP, 1000, 0);
        animate_x_from(m_controller->sports_screen().get(), SCREEN_SIZE + TRANSITION_GAP, 0, 1000, 0);
    }
    if (dir == lv::kDir::right) {
        animate_x_from(m_controller->weather_screen().get(), -SCREEN_SIZE - TRANSITION_GAP, 0, 1000, 0);
        animate_x(m_screen, SCREEN_SIZE + TRANSITION_GAP, 1000, 0);
    }
}

// SportsScreen::on_gesture
inline void SportsScreen::on_gesture(lv::Event e) {
    (void)e;
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

    if (dir == lv::kDir::left) {
        animate_x(m_screen, -SCREEN_SIZE - TRANSITION_GAP, 1000, 0);
        animate_x_from(m_controller->music_screen().get(), SCREEN_SIZE + TRANSITION_GAP, 0, 1000, 0);
    }
    if (dir == lv::kDir::right) {
        animate_x_from(m_controller->health_screen().get(), -SCREEN_SIZE - TRANSITION_GAP, 0, 1000, 0);
        animate_x(m_screen, SCREEN_SIZE + TRANSITION_GAP, 1000, 0);
    }
}

// MusicScreen::on_gesture
inline void MusicScreen::on_gesture(lv::Event e) {
    (void)e;
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

    if (dir == lv::kDir::left) {
        animate_x(m_screen, -SCREEN_SIZE, 1000, 0);
        animate_arc(m_controller->arc_cont(), ArcAnimation::EXPAND_RIGHT, 700, 300);
        animate_opa(m_controller->main_arc(), 255, 500, 500);
        animate_opa(m_controller->overlay(), 0, 100, 0);
    }
    if (dir == lv::kDir::right) {
        animate_x_from(m_controller->sports_screen().get(), -SCREEN_SIZE - TRANSITION_GAP, 0, 1000, 0);
        animate_x(m_screen, SCREEN_SIZE + TRANSITION_GAP, 1000, 0);
    }
}

} // namespace smartwatch

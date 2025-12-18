#pragma once

/**
 * @file ebike_demo.hpp
 * @brief Main E-Bike demo class - C++ port
 */

#include <lv/lv.hpp>
#include "ebike_private.hpp"
#include "ebike_home.hpp"
#include "ebike_settings.hpp"
#include "ebike_stats.hpp"
#include "translations/lv_i18n.h"
#include <lv/core/i18n.hpp>  // Must be after lv_i18n.h

namespace ebike {

/**
 * @brief Main E-Bike demo application
 *
 * This is a C++ port of the official LVGL E-Bike demo.
 * It demonstrates:
 * - Home screen with speedometer and info cards
 * - Settings screen with controls
 * - Statistics screen with chart
 * - Navigation via icon menu bar
 */
class EbikeDemo {
    lv::Box m_main_cont;
    lv::Box m_menu_cont;

    HomePage m_home_page;
    SettingsPage m_settings_page;
    StatsPage m_stats_page;

    lv::IntState m_language_subject{0};

    lv::Image m_menu_icons[3];

    enum class Page {
        SETTINGS = 0,
        STATS = 1,
        HOME = 2
    };

    Page m_current_page = Page::HOME;
    int32_t m_current_lang = 0;  // Track current language to avoid re-entry

public:
    void create() {
        // Initialize i18n
        lv::i18n::init(lv_i18n_language_pack);

        // Initialize pages
        m_home_page.init();
        m_stats_page.init();

        // Use simple theme to remove default borders
        auto display = lv::Display::get_default();
        display.set_theme(lv::theme_simple_init(display.get()));

        auto screen = lv::screen_active();

        // Setup screen - now using fluent API since Screen inherits mixins
        screen.flex_flow(lv::kFlexFlow::row)
              .remove_flag(lv::kFlag::scrollable)
              .padding(0)
              .gap(0)
              .text_color(lv::colors::white())
              .bg_color(lv::rgb(0xffeeaa));

        // Background image
        lv::Image::create(screen)
            .src(&img_ebike_bg)
            .align(lv::kAlign::center)
            .add_flag(lv::kFlag::ignore_layout);

        // Main content container
        m_main_cont = lv::Box::create(screen)
            .fill()
            .bg_opa(lv::opa::transp)
            .grow()
            .padding(0);

        // Create menu bar
        create_menu_bar(screen);

        // Add language observer to switch locale and refresh page
        m_language_subject.observe<&EbikeDemo::on_language_changed>(this);

        // Start with home page
        show_page(Page::HOME);
    }

    void destroy() {
        m_home_page.deinit();
        m_stats_page.deinit();
        // m_language_subject cleanup is automatic (RAII)
    }

private:
    void create_menu_bar(lv::ObjectView screen) {
        m_menu_cont = lv::Box::create(screen)
            .size(44, lv::pct(100))
            .bg_color(lv::colors::black())
            .padding(0)
            .gap(16)
            .flex_flow(lv::kFlexFlow::column)
            .flex_align(lv::kFlexAlign::center, lv::kFlexAlign::center, lv::kFlexAlign::center);

        // Settings icon
        m_menu_icons[0] = lv::Image::create(m_menu_cont)
            .src(&img_ebike_settings)
            .size(44, 44)
            .clickable()
            .ext_click_area(8)
            .image_opa(lv::opa::_50)
            .on_click<&EbikeDemo::on_settings_click>(this);

        // Stats icon
        m_menu_icons[1] = lv::Image::create(m_menu_cont)
            .src(&img_ebike_stats)
            .size(44, 44)
            .clickable()
            .ext_click_area(8)
            .image_opa(lv::opa::_50)
            .on_click<&EbikeDemo::on_stats_click>(this);

        // Home icon
        m_menu_icons[2] = lv::Image::create(m_menu_cont)
            .src(&img_ebike_home)
            .size(44, 44)
            .clickable()
            .ext_click_area(8)
            .image_opa(lv::opa::_100)  // Start with home selected
            .on_click<&EbikeDemo::on_home_click>(this);
    }

    void show_page(Page page) {
        // Clean current content
        m_main_cont.clean();

        // Update icon opacity
        for (int i = 0; i < 3; i++) {
            m_menu_icons[i].image_opa(lv::opa::_50);
        }
        m_menu_icons[static_cast<int>(page)].image_opa(lv::opa::_100);

        // Show selected page
        m_current_page = page;
        switch (page) {
            case Page::SETTINGS:
                m_settings_page.create(m_main_cont, m_language_subject.subject());
                break;
            case Page::STATS:
                m_stats_page.create(m_main_cont);
                break;
            case Page::HOME:
                m_home_page.create(m_main_cont);
                break;
        }
    }

    // Event callbacks
    void on_settings_click() { show_page(Page::SETTINGS); }
    void on_stats_click() { show_page(Page::STATS); }
    void on_home_click() { show_page(Page::HOME); }

    void on_language_changed(int32_t lang) {
        // Avoid re-entry when page is recreated
        if (lang == m_current_lang) return;
        m_current_lang = lang;

        // Set locale based on selection
        switch (lang) {
            case 0: lv::i18n::set_locale("en"); break;
            case 1: lv::i18n::set_locale("zh"); break;
            case 2: lv::i18n::set_locale("ar"); break;
        }

        // Refresh current page to update translations
        show_page(m_current_page);
    }
};

} // namespace ebike

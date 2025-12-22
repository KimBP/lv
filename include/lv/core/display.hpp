#pragma once

/**
 * @file display.hpp
 * @brief Display and input device wrappers
 *
 * Provides clean C++ abstractions for LVGL display backends.
 */

#include <lvgl.h>
#include "object.hpp"
#include <cstdint>

namespace lv {

/**
 * @brief Non-owning wrapper for LVGL display
 */
class Display {
    lv_display_t* m_display;

public:
    constexpr Display(lv_display_t* d) noexcept : m_display(d) {}

    [[nodiscard]] constexpr lv_display_t* get() const noexcept { return m_display; }
    [[nodiscard]] constexpr operator lv_display_t*() const noexcept { return m_display; }

    [[nodiscard]] int32_t width() const noexcept {
        return lv_display_get_horizontal_resolution(m_display);
    }

    [[nodiscard]] int32_t height() const noexcept {
        return lv_display_get_vertical_resolution(m_display);
    }

    [[nodiscard]] ObjectView screen_active() const noexcept {
        return ObjectView(lv_display_get_screen_active(m_display));
    }

    /// Set theme for this display
    Display& set_theme(lv_theme_t* theme) noexcept {
        lv_display_set_theme(m_display, theme);
        return *this;
    }

    /// Get current theme
    [[nodiscard]] lv_theme_t* get_theme() const noexcept {
        return lv_display_get_theme(m_display);
    }

    /// Get default display
    [[nodiscard]] static Display get_default() noexcept {
        return Display(lv_display_get_default());
    }
};


#if LV_USE_X11
/**
 * @brief X11 display backend
 *
 * Creates a native X11 window for Linux desktop.
 *
 * Usage:
 * @code
 * lv::init();
 * auto display = lv::X11Display("My App", 800, 480);
 * // ... create UI ...
 * lv::run();
 * @endcode
 *
 * With custom mouse cursor:
 * @code
 * LV_IMAGE_DECLARE(my_cursor);
 * lv::X11Display display("My App", 800, 480, &my_cursor);
 * @endcode
 */
class X11Display : public Display {
public:
    /// Create X11 window with optional mouse cursor image
    /// @param title Window title
    /// @param width Window width in pixels
    /// @param height Window height in pixels
    /// @param cursor Optional cursor image (nullptr = no visible cursor)
    X11Display(const char* title, int32_t width, int32_t height,
               const lv_image_dsc_t* cursor = nullptr)
        : Display(lv_x11_window_create(title, width, height)) {
        lv_x11_inputs_create(get(), cursor);
    }
};
#endif


#if LV_USE_SDL
/**
 * @brief SDL display backend
 *
 * Creates an SDL window for cross-platform development.
 *
 * Usage:
 * @code
 * lv::init();
 * auto display = lv::SDLDisplay(800, 480);
 * // ... create UI ...
 * lv::run();
 * @endcode
 */
class SDLDisplay : public Display {
public:
    SDLDisplay(int32_t width, int32_t height)
        : Display(lv_sdl_window_create(width, height)) {
        lv_sdl_mouse_create();
    }
};
#endif


#if LV_USE_LINUX_FBDEV
/**
 * @brief Linux framebuffer display backend
 */
class FBDisplay : public Display {
public:
    FBDisplay()
        : Display(lv_linux_fbdev_create()) {
    }
};
#endif


#if LV_USE_LINUX_DRM
/**
 * @brief Linux DRM/KMS display backend
 */
class DRMDisplay : public Display {
public:
    DRMDisplay()
        : Display(lv_linux_drm_create()) {
    }
};
#endif

} // namespace lv

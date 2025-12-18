#pragma once

/**
 * @file color.hpp
 * @brief Color utilities for LVGL
 *
 * Provides color constants, conversion functions, and utilities.
 *
 * ## Naming Conventions
 *
 * ### Color Constructors
 * Use short, intuitive names for creating colors in the same color space:
 *
 * ```cpp
 * lv::rgb(255, 0, 0)      // RGB from components (r, g, b)
 * lv::rgb(0xFF0000)       // RGB from hex value
 * lv::rgb3(0xF00)         // RGB from 3-digit hex (0xRGB → 0xRRGGBB)
 * ```
 *
 * ### Color Space Conversions
 * Use `source_to_target` naming for color space conversions:
 *
 * ```cpp
 * lv::hsv_to_rgb(h, s, v) // HSV → RGB (from components)
 * lv::hsv_to_rgb(hsv)     // HSV → RGB (from struct)
 * lv::rgb_to_hsv(color)   // RGB → HSV
 * ```
 *
 * ### Color Constants
 * Named colors are in the `lv::colors` namespace:
 *
 * ```cpp
 * lv::colors::white()
 * lv::colors::black()
 * lv::colors::red()
 * lv::colors::material_blue()
 * ```
 *
 * ### Opacity
 * Opacity values are in the `lv::opa` namespace:
 *
 * ```cpp
 * lv::opa::cover      // 100% (fully opaque)
 * lv::opa::transp     // 0% (fully transparent)
 * lv::opa::_50        // 50%
 * lv::opa::pct(75)    // 75% (from percentage)
 * ```
 */

#include <lvgl.h>
#include <cstdint>
#include <algorithm>

namespace lv {

// ==================== Color Types ====================

/// Color type (alias for lv_color_t)
using Color = lv_color_t;

/// Opacity type (alias for lv_opa_t)
using Opacity = lv_opa_t;

/// HSV color type (alias for lv_color_hsv_t)
using ColorHSV = lv_color_hsv_t;

// ==================== Color Creation ====================

/// Create color from RGB components (0-255)
inline lv_color_t rgb(uint8_t r, uint8_t g, uint8_t b) noexcept {
    return lv_color_make(r, g, b);
}

/// Create color from hex value (0xRRGGBB)
inline lv_color_t rgb(uint32_t hex) noexcept {
    return lv_color_hex(hex);
}

/// Create color from 3-digit hex (0xRGB -> 0xRRGGBB)
inline lv_color_t rgb3(uint32_t hex) noexcept {
    return lv_color_hex3(hex);
}

// ==================== Color Constants ====================

namespace colors {
    // Basic colors
    inline lv_color_t white() noexcept { return lv_color_white(); }
    inline lv_color_t black() noexcept { return lv_color_black(); }
    inline lv_color_t red() noexcept { return rgb(255, 0, 0); }
    inline lv_color_t green() noexcept { return rgb(0, 255, 0); }
    inline lv_color_t blue() noexcept { return rgb(0, 0, 255); }
    inline lv_color_t yellow() noexcept { return rgb(255, 255, 0); }
    inline lv_color_t cyan() noexcept { return rgb(0, 255, 255); }
    inline lv_color_t magenta() noexcept { return rgb(255, 0, 255); }

    // Grays
    inline lv_color_t gray() noexcept { return rgb(128, 128, 128); }
    inline lv_color_t light_gray() noexcept { return rgb(192, 192, 192); }
    inline lv_color_t dark_gray() noexcept { return rgb(64, 64, 64); }
    inline lv_color_t silver() noexcept { return rgb(192, 192, 192); }

    // Extended palette
    inline lv_color_t orange() noexcept { return rgb(255, 165, 0); }
    inline lv_color_t pink() noexcept { return rgb(255, 192, 203); }
    inline lv_color_t purple() noexcept { return rgb(128, 0, 128); }
    inline lv_color_t violet() noexcept { return rgb(238, 130, 238); }
    inline lv_color_t indigo() noexcept { return rgb(75, 0, 130); }
    inline lv_color_t brown() noexcept { return rgb(165, 42, 42); }
    inline lv_color_t maroon() noexcept { return rgb(128, 0, 0); }
    inline lv_color_t olive() noexcept { return rgb(128, 128, 0); }
    inline lv_color_t teal() noexcept { return rgb(0, 128, 128); }
    inline lv_color_t navy() noexcept { return rgb(0, 0, 128); }
    inline lv_color_t lime() noexcept { return rgb(0, 255, 0); }
    inline lv_color_t aqua() noexcept { return rgb(0, 255, 255); }

    // Material Design colors
    inline lv_color_t material_red() noexcept { return rgb(0xF44336); }
    inline lv_color_t material_pink() noexcept { return rgb(0xE91E63); }
    inline lv_color_t material_purple() noexcept { return rgb(0x9C27B0); }
    inline lv_color_t material_deep_purple() noexcept { return rgb(0x673AB7); }
    inline lv_color_t material_indigo() noexcept { return rgb(0x3F51B5); }
    inline lv_color_t material_blue() noexcept { return rgb(0x2196F3); }
    inline lv_color_t material_light_blue() noexcept { return rgb(0x03A9F4); }
    inline lv_color_t material_cyan() noexcept { return rgb(0x00BCD4); }
    inline lv_color_t material_teal() noexcept { return rgb(0x009688); }
    inline lv_color_t material_green() noexcept { return rgb(0x4CAF50); }
    inline lv_color_t material_light_green() noexcept { return rgb(0x8BC34A); }
    inline lv_color_t material_lime() noexcept { return rgb(0xCDDC39); }
    inline lv_color_t material_yellow() noexcept { return rgb(0xFFEB3B); }
    inline lv_color_t material_amber() noexcept { return rgb(0xFFC107); }
    inline lv_color_t material_orange() noexcept { return rgb(0xFF9800); }
    inline lv_color_t material_deep_orange() noexcept { return rgb(0xFF5722); }
    inline lv_color_t material_brown() noexcept { return rgb(0x795548); }
    inline lv_color_t material_grey() noexcept { return rgb(0x9E9E9E); }
    inline lv_color_t material_blue_grey() noexcept { return rgb(0x607D8B); }

    // Transparent (for opacity operations)
    inline lv_color_t transparent() noexcept { return black(); } // Use with LV_OPA_TRANSP
} // namespace colors

// ==================== Color Operations ====================

/// Mix two colors with a ratio (0 = color1, 255 = color2)
inline lv_color_t color_mix(lv_color_t c1, lv_color_t c2, uint8_t ratio) noexcept {
    return lv_color_mix(c2, c1, ratio);
}

/// Lighten a color (ratio: 0 = original, 255 = white)
inline lv_color_t color_lighten(lv_color_t color, uint8_t ratio) noexcept {
    return lv_color_mix(lv_color_white(), color, ratio);
}

/// Darken a color (ratio: 0 = original, 255 = black)
inline lv_color_t color_darken(lv_color_t color, uint8_t ratio) noexcept {
    return lv_color_mix(lv_color_black(), color, ratio);
}

/// Get brightness of a color (0-255)
inline uint8_t color_brightness(lv_color_t color) noexcept {
    return lv_color_brightness(color);
}

/// Convert HSV to RGB color
/// @param h Hue (0-359)
/// @param s Saturation (0-100)
/// @param v Value (0-100)
inline lv_color_t hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v) noexcept {
    return lv_color_hsv_to_rgb(h, s, v);
}

/// Convert HSV struct to RGB color
inline lv_color_t hsv_to_rgb(lv_color_hsv_t hsv) noexcept {
    return lv_color_hsv_to_rgb(hsv.h, hsv.s, hsv.v);
}

/// Convert RGB color to HSV
inline lv_color_hsv_t rgb_to_hsv(lv_color_t color) noexcept {
    return lv_color_to_hsv(color);
}

/// Interpolate between two colors
/// @param from Start color
/// @param to End color
/// @param t Interpolation factor (0.0 = from, 1.0 = to)
inline lv_color_t color_lerp(lv_color_t from, lv_color_t to, float t) noexcept {
    uint8_t ratio = static_cast<uint8_t>(std::clamp(t, 0.0f, 1.0f) * 255);
    return lv_color_mix(to, from, ratio);
}

/// Create a gradient color at position
/// @param start Start color
/// @param end End color
/// @param pos Position (0-255)
inline lv_color_t gradient_color(lv_color_t start, lv_color_t end, uint8_t pos) noexcept {
    return lv_color_mix(end, start, pos);
}

// ==================== Opacity Helpers ====================

namespace opa {
    constexpr lv_opa_t transp = LV_OPA_TRANSP;   // 0%
    constexpr lv_opa_t _0 = LV_OPA_0;
    constexpr lv_opa_t _10 = LV_OPA_10;
    constexpr lv_opa_t _20 = LV_OPA_20;
    constexpr lv_opa_t _30 = LV_OPA_30;
    constexpr lv_opa_t _40 = LV_OPA_40;
    constexpr lv_opa_t _50 = LV_OPA_50;
    constexpr lv_opa_t _60 = LV_OPA_60;
    constexpr lv_opa_t _70 = LV_OPA_70;
    constexpr lv_opa_t _80 = LV_OPA_80;
    constexpr lv_opa_t _90 = LV_OPA_90;
    constexpr lv_opa_t _100 = LV_OPA_100;
    constexpr lv_opa_t cover = LV_OPA_COVER;     // 100%

    /// Create opacity from percentage (0-100)
    constexpr lv_opa_t pct(uint8_t percent) noexcept {
        return static_cast<lv_opa_t>((percent * 255) / 100);
    }
} // namespace opa

// ==================== Color Palette Generator ====================

/**
 * @brief Generate a color palette from a base color
 */
struct Palette {
    lv_color_t base;

    explicit Palette(lv_color_t color) noexcept : base(color) {}
    explicit Palette(uint32_t hex_color) noexcept : base(rgb(hex_color)) {}

    /// Lightest variant (50)
    [[nodiscard]] lv_color_t lightest() const noexcept { return color_lighten(base, 200); }
    /// Light variant (200)
    [[nodiscard]] lv_color_t light() const noexcept { return color_lighten(base, 128); }
    /// Normal variant (500)
    [[nodiscard]] lv_color_t normal() const noexcept { return base; }
    /// Dark variant (700)
    [[nodiscard]] lv_color_t dark() const noexcept { return color_darken(base, 64); }
    /// Darkest variant (900)
    [[nodiscard]] lv_color_t darkest() const noexcept { return color_darken(base, 128); }

    /// Get variant by index (0 = lightest, 4 = darkest)
    [[nodiscard]] lv_color_t variant(int index) const noexcept {
        switch (index) {
            case 0: return lightest();
            case 1: return light();
            case 2: return normal();
            case 3: return dark();
            case 4: return darkest();
            default: return normal();
        }
    }
};

} // namespace lv

#pragma once

/**
 * @file ebike_private.hpp
 * @brief Private definitions for ebike demo - C++ port
 */

#include <lv/lv.hpp>

// Font declarations - these are in global namespace (from generated C files)
LV_FONT_DECLARE(font_ebike_inter_14)
LV_FONT_DECLARE(font_ebike_trump_24)
LV_FONT_DECLARE(font_ebike_trump_48)
LV_FONT_DECLARE(font_ebike_130)

// Image declarations - these are in global namespace (from generated C files)
LV_IMAGE_DECLARE(img_ebike_bg);
LV_IMAGE_DECLARE(img_ebike_scale);
LV_IMAGE_DECLARE(img_ebike_settings);
LV_IMAGE_DECLARE(img_ebike_stats);
LV_IMAGE_DECLARE(img_ebike_home);
LV_IMAGE_DECLARE(img_ebike_arrow_left);
LV_IMAGE_DECLARE(img_ebike_arrow_right);
LV_IMAGE_DECLARE(img_ebike_arrow_left_2);
LV_IMAGE_DECLARE(img_ebike_arrow_right_2);
LV_IMAGE_DECLARE(img_ebike_lamp);
LV_IMAGE_DECLARE(img_ebike_clock);
LV_IMAGE_DECLARE(img_ebike_dropdown_icon);
LV_IMAGE_DECLARE(img_ebike_settings_large);
LV_IMAGE_DECLARE(img_ebike_stats_large);

namespace ebike {

// Colors used throughout the demo
inline constexpr uint32_t COLOR_TURQUOISE = 0x55FFEB;
inline constexpr uint32_t COLOR_LIME = 0x91FF3B;

inline lv::Color turquoise() { return lv::rgb(COLOR_TURQUOISE); }
inline lv::Color lime() { return lv::rgb(COLOR_LIME); }

inline const lv_font_t* font_small() { return &font_ebike_inter_14; }
inline const lv_font_t* font_medium() { return &font_ebike_trump_24; }
inline const lv_font_t* font_large() { return &font_ebike_trump_48; }
inline const lv_font_t* font_speed() { return &font_ebike_130; }

} // namespace ebike

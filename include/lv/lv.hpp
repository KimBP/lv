#pragma once

/**
 * @file lv.hpp
 * @brief Main header for lv:: C++20 bindings for LVGL
 *
 * Include this single header to get access to all lv:: bindings.
 *
 * Features:
 * - Zero-cost abstractions (no overhead vs raw LVGL C API)
 * - Embedded-friendly (minimal/no heap allocations)
 * - Modern C++20 API (concepts, CRTP, fluent interfaces)
 * - Reactive state with LVGL observer integration
 *
 * @code
 * #include <lv/lv.hpp>
 *
 * class MyApp : public lv::Component<MyApp> {
 *     lv::State<int> counter{0};
 *
 * public:
 *     lv::ObjectView build(lv::ObjectView parent) {
 *         auto root = lv::vbox(parent).padding(16).gap(8);
 *
 *         lv::Label::create(root)
 *             .bind_text(counter, "Count: %d");
 *
 *         lv::Button::create(root)
 *             .text("Click me")
 *             .on_click<&MyApp::on_click>(this);
 *
 *         return root;
 *     }
 *
 * private:
 *     void on_click(lv::Event) {
 *         ++counter;
 *     }
 * };
 * @endcode
 */

// LVGL base
#include <lvgl.h>

// Core
#include "core/constants.hpp"
#include "core/object.hpp"
#include "core/event.hpp"
#include "core/style.hpp"
#include "core/color.hpp"
#include "core/font.hpp"
#include "core/display.hpp"
#include "core/app.hpp"
#include "core/component.hpp"
#include "core/anim.hpp"
#include "core/theme.hpp"
#include "core/screen.hpp"
#include "core/indev.hpp"
#include "core/focus.hpp"
#include "core/timer.hpp"
#include "core/image.hpp"
#include "core/font_loader.hpp"
#include "core/string_utils.hpp"

// Misc utilities
#include "misc/gradient.hpp"

// Internationalization (requires LV_USE_TRANSLATION)
#if LV_USE_TRANSLATION
#include "core/translation.hpp"
#endif

// Layout
#include "layout/flex.hpp"
#include "layout/grid.hpp"

// Widgets - Basic
#include "widgets/box.hpp"
#include "widgets/label.hpp"
#include "widgets/button.hpp"
#include "widgets/image.hpp"
#include "widgets/line.hpp"
#include "widgets/led.hpp"

// Widgets - Input
#include "widgets/switch.hpp"
#include "widgets/slider.hpp"
#include "widgets/dropdown.hpp"
#include "widgets/checkbox.hpp"
#include "widgets/roller.hpp"
#include "widgets/textarea.hpp"
#include "widgets/spinbox.hpp"
#include "widgets/keyboard.hpp"
#include "widgets/buttonmatrix.hpp"

// Widgets - Display
#include "widgets/arc.hpp"
#include "widgets/bar.hpp"
#include "widgets/spinner.hpp"
#include "widgets/chart.hpp"
#include "widgets/scale.hpp"
#include "widgets/table.hpp"
#include "widgets/canvas.hpp"
#include "widgets/animimage.hpp"
#include "widgets/span.hpp"

// Widgets - Containers
#include "widgets/list.hpp"
#include "widgets/menu.hpp"
#include "widgets/tabview.hpp"
#include "widgets/tileview.hpp"
#include "widgets/win.hpp"
#include "widgets/msgbox.hpp"
#include "widgets/calendar.hpp"
#include "widgets/imagebutton.hpp"

// Widgets - Other (conditional)
#if LV_USE_FILE_EXPLORER
#include "widgets/file_explorer.hpp"
#endif

#if LV_USE_ARCLABEL
#include "widgets/arclabel.hpp"
#endif

#if LV_USE_IME_PINYIN
#include "widgets/ime_pinyin.hpp"
#endif

#if LV_USE_3DTEXTURE
#include "widgets/texture3d.hpp"
#endif

// Lottie animation (requires ThorVG)
#if LV_USE_LOTTIE
#include "widgets/lottie.hpp"
#endif

// Libs (optional)
#if LV_USE_QRCODE
#include "libs/qrcode.hpp"
#endif

#if LV_USE_BARCODE
#include "libs/barcode.hpp"
#endif

#if LV_USE_GIF
#include "libs/gif.hpp"
#endif

#if LV_USE_GLTF
#include "libs/gltf.hpp"
#endif

// State (requires LV_USE_OBSERVER)
#if LV_USE_OBSERVER
#include "core/state.hpp"
#endif

/**
 * @namespace lv
 * @brief Modern C++20 bindings for LVGL
 *
 * The lv:: namespace provides zero-cost C++ wrappers for LVGL.
 *
 * ## Key Components
 *
 * ### Object Wrappers
 * - `ObjectView` - Non-owning view of lv_obj_t (zero overhead)
 * - `Object` - Owning wrapper with RAII (move-only)
 *
 * ### Widgets
 * - `Label` - Text display
 * - `Button` - Clickable button
 * - `Flex` - Flex layout container
 *
 * ### Layout DSL
 * - `hbox()` - Horizontal flex container
 * - `vbox()` - Vertical flex container
 *
 * ### Reactive State
 * - `State<T>` - Reactive value (triggers UI updates)
 * - `IntState`, `BoolState`, `ColorState` - Common specializations
 *
 * ### Components
 * - `Component<Derived>` - CRTP base for UI components
 * - `ScreenComponent<Derived>` - Full-screen component
 *
 * ### Styling
 * - `Style` - RAII style object
 * - `StyleMixin` - Inline style setters
 * - `colors::` - Common color constants
 *
 * ### Events
 * - `EventMixin` - Zero-cost event callbacks
 * - `on_click()`, `on_value_changed()`, etc.
 *
 * ## Memory Model
 *
 * All wrappers are designed for zero or minimal heap allocation:
 * - Widget wrappers are just pointers (8 bytes)
 * - State<T> embeds lv_subject_t (no heap)
 * - Style embeds lv_style_t (no heap)
 * - Event callbacks use user_data (no heap)
 *
 * ## Thread Safety
 *
 * LVGL is single-threaded. All lv:: operations must be called
 * from the same thread as lv_timer_handler().
 */
namespace lv {

// ==================== Version Info ====================

/// Library version
constexpr int VERSION_MAJOR = 0;
constexpr int VERSION_MINOR = 1;
constexpr int VERSION_PATCH = 0;

/// Get version string
constexpr const char* version() noexcept {
    return "0.1.0";
}

// Screen helpers are provided by core/screen.hpp

/// Get the layer_top (always on top)
inline ObjectView layer_top() noexcept {
    return ObjectView(lv_layer_top());
}

/// Get the layer_sys (system layer)
inline ObjectView layer_sys() noexcept {
    return ObjectView(lv_layer_sys());
}

// ==================== Display Helpers ====================

/// Get default display width
inline int32_t display_width() noexcept {
    return lv_display_get_horizontal_resolution(lv_display_get_default());
}

/// Get default display height
inline int32_t display_height() noexcept {
    return lv_display_get_vertical_resolution(lv_display_get_default());
}

// ==================== Timer/Task Helpers ====================

/// Run LVGL timer handler (call periodically)
inline uint32_t timer_handler() noexcept {
    return lv_timer_handler();
}

/// Get elapsed milliseconds
inline uint32_t tick_get() noexcept {
    return lv_tick_get();
}

} // namespace lv

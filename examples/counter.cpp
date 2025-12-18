/**
 * @file counter.cpp
 * @brief Counter example demonstrating lv:: C++ bindings
 *
 * This example shows:
 * - Component pattern with CRTP
 * - Reactive State<T> for automatic UI updates
 * - Zero-cost event callbacks with member functions
 * - Fluent layout DSL (vbox, hbox)
 * - Pure C++ API - no raw pointers or C function calls
 *
 * Build:
 *   cmake -B build -DLVGL_DIR=../lvgl
 *   cmake --build build
 *   ./build/examples/counter
 */

#include <lv/lv.hpp>

#if LV_USE_OBSERVER

/**
 * @brief Counter application component
 *
 * A simple counter with increment/decrement buttons.
 * Demonstrates reactive state binding and event handling.
 */
class CounterApp : public lv::Component<CounterApp> {
    lv::State<int> m_count{0};

public:
    lv::ObjectView build(lv::ObjectView parent) {
        auto root = lv::vbox(parent)
            .fill()
            .padding(20)
            .gap(16)
            .center_content();

        // Title label
        lv::Label::create(root)
            .text("Counter Example")
            .font(lv::fonts::montserrat_20)
            .center_text();

        // Counter value label - bound to state for auto-update
        lv::Label::create(root)
            .bind_text(m_count, "Count: %d")
            .font(lv::fonts::montserrat_28)
            .center_text();

        // Button row
        auto buttons = lv::hbox(root)
            .gap(20)
            .size_content();

        // Decrement button
        lv::Button::create(buttons)
            .size(60, 60)
            .radius(10)
            .text("-")
            .on_click<&CounterApp::decrement>(this);

        // Reset button
        lv::Button::create(buttons)
            .size(80, 60)
            .radius(10)
            .text("Reset")
            .on_click<&CounterApp::reset>(this);

        // Increment button
        lv::Button::create(buttons)
            .size(60, 60)
            .radius(10)
            .text("+")
            .on_click<&CounterApp::increment>(this);

        return root;
    }

private:
    // State<T> update methods:
    // - operator++/-- : modify and notify observers (triggers UI update)
    // - set(value)    : assign new value and notify observers
    // - notify()      : manually trigger observers without changing value
    //                   (useful when underlying data changed externally)
    void increment(lv::Event) { ++m_count; }
    void decrement(lv::Event) { --m_count; }
    void reset(lv::Event) { m_count.set(0); }
};

#endif // LV_USE_OBSERVER


// ==================== Main ====================

int main() {
    lv::init();

    // Create display based on available backend
#if LV_USE_X11
    lv::X11Display display("Counter Example", 800, 480);
#elif LV_USE_SDL
    lv::SDLDisplay display(800, 480);
#else
    #error "No display backend enabled. Enable LV_USE_X11 or LV_USE_SDL in lv_conf.h"
#endif

#if LV_USE_OBSERVER
    CounterApp app;
    app.mount(lv::screen_active());
#else
    lv::Label::create(lv::screen_active())
        .text("Enable LV_USE_OBSERVER for this demo")
        .center();
#endif

    // Run the main loop (never returns)
    lv::run();
}

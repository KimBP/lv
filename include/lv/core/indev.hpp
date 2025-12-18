#pragma once

/**
 * @file indev.hpp
 * @brief Zero-cost input device wrapper for LVGL
 *
 * Provides C++ API for input devices (touch, mouse, keyboard, encoder).
 */

#include <lvgl.h>
#include "object.hpp"

namespace lv {

// ==================== Input Device Types ====================

namespace indev_type {
    constexpr auto none = LV_INDEV_TYPE_NONE;
    constexpr auto pointer = LV_INDEV_TYPE_POINTER;
    constexpr auto keypad = LV_INDEV_TYPE_KEYPAD;
    constexpr auto button = LV_INDEV_TYPE_BUTTON;
    constexpr auto encoder = LV_INDEV_TYPE_ENCODER;
} // namespace indev_type

// ==================== Input Device States ====================

namespace indev_state {
    constexpr auto released = LV_INDEV_STATE_RELEASED;
    constexpr auto pressed = LV_INDEV_STATE_PRESSED;
} // namespace indev_state

// ==================== Keys ====================

namespace key {
    constexpr uint32_t up = LV_KEY_UP;
    constexpr uint32_t down = LV_KEY_DOWN;
    constexpr uint32_t right = LV_KEY_RIGHT;
    constexpr uint32_t left = LV_KEY_LEFT;
    constexpr uint32_t esc = LV_KEY_ESC;
    constexpr uint32_t del = LV_KEY_DEL;
    constexpr uint32_t backspace = LV_KEY_BACKSPACE;
    constexpr uint32_t enter = LV_KEY_ENTER;
    constexpr uint32_t next = LV_KEY_NEXT;
    constexpr uint32_t prev = LV_KEY_PREV;
    constexpr uint32_t home = LV_KEY_HOME;
    constexpr uint32_t end = LV_KEY_END;
} // namespace key

/**
 * @brief Input device wrapper
 *
 * Wraps LVGL's input device system.
 */
class Indev {
    lv_indev_t* m_indev;
    bool m_owned;

public:
    /// Create a new input device
    Indev() noexcept : m_indev(lv_indev_create()), m_owned(true) {}

    /// Wrap existing input device (non-owning)
    explicit Indev(lv_indev_t* indev) noexcept : m_indev(indev), m_owned(false) {}

    ~Indev() {
        if (m_owned && m_indev) {
            lv_indev_delete(m_indev);
        }
    }

    // Non-copyable
    Indev(const Indev&) = delete;
    Indev& operator=(const Indev&) = delete;

    // Moveable
    Indev(Indev&& other) noexcept : m_indev(other.m_indev), m_owned(other.m_owned) {
        other.m_indev = nullptr;
        other.m_owned = false;
    }

    Indev& operator=(Indev&& other) noexcept {
        if (this != &other) {
            if (m_owned && m_indev) {
                lv_indev_delete(m_indev);
            }
            m_indev = other.m_indev;
            m_owned = other.m_owned;
            other.m_indev = nullptr;
            other.m_owned = false;
        }
        return *this;
    }

    /// Get underlying input device pointer
    [[nodiscard]] lv_indev_t* get() noexcept { return m_indev; }
    [[nodiscard]] const lv_indev_t* get() const noexcept { return m_indev; }

    // ==================== Configuration ====================

    /// Set input device type
    Indev& type(lv_indev_type_t t) noexcept {
        lv_indev_set_type(m_indev, t);
        return *this;
    }

    /// Set as pointer (touch/mouse)
    Indev& as_pointer() noexcept {
        return type(LV_INDEV_TYPE_POINTER);
    }

    /// Set as keypad
    Indev& as_keypad() noexcept {
        return type(LV_INDEV_TYPE_KEYPAD);
    }

    /// Set as encoder
    Indev& as_encoder() noexcept {
        return type(LV_INDEV_TYPE_ENCODER);
    }

    /// Set as button
    Indev& as_button() noexcept {
        return type(LV_INDEV_TYPE_BUTTON);
    }

    /// Set read callback
    Indev& read_cb(lv_indev_read_cb_t cb) noexcept {
        lv_indev_set_read_cb(m_indev, cb);
        return *this;
    }

    /// Set user data
    Indev& user_data(void* data) noexcept {
        lv_indev_set_user_data(m_indev, data);
        return *this;
    }

    /// Get user data
    [[nodiscard]] void* user_data() const noexcept {
        return lv_indev_get_user_data(m_indev);
    }

    /// Set display
    Indev& display(lv_display_t* disp) noexcept {
        lv_indev_set_display(m_indev, disp);
        return *this;
    }

    // ==================== Group ====================

    /// Set group (for keypad/encoder navigation)
    Indev& group(lv_group_t* grp) noexcept {
        lv_indev_set_group(m_indev, grp);
        return *this;
    }

    /// Get group
    [[nodiscard]] lv_group_t* group() const noexcept {
        return lv_indev_get_group(m_indev);
    }

    // ==================== State ====================

    /// Enable or disable
    Indev& enable(bool en = true) noexcept {
        lv_indev_enable(m_indev, en);
        return *this;
    }

    /// Disable
    Indev& disable() noexcept {
        return enable(false);
    }

    /// Get type
    [[nodiscard]] lv_indev_type_t get_type() const noexcept {
        return lv_indev_get_type(m_indev);
    }

    /// Get current point (for pointer devices)
    [[nodiscard]] lv_point_t point() const noexcept {
        lv_point_t p;
        lv_indev_get_point(m_indev, &p);
        return p;
    }

    /// Get current key (for keypad devices)
    [[nodiscard]] uint32_t get_key() const noexcept {
        return lv_indev_get_key(m_indev);
    }

    /// Get scroll direction
    [[nodiscard]] lv_dir_t scroll_dir() const noexcept {
        return lv_indev_get_scroll_dir(m_indev);
    }

    /// Get scroll object
    [[nodiscard]] ObjectView scroll_obj() const noexcept {
        return ObjectView(lv_indev_get_scroll_obj(m_indev));
    }

    /// Get gesture direction
    [[nodiscard]] lv_dir_t gesture_dir() const noexcept {
        return lv_indev_get_gesture_dir(m_indev);
    }

    // ==================== Cursor ====================

    /// Set cursor object (for pointer devices)
    Indev& cursor(ObjectView obj) noexcept {
        lv_indev_set_cursor(m_indev, obj.get());
        return *this;
    }

    // ==================== Long Press ====================

    /// Set long press time
    Indev& long_press_time(uint32_t ms) noexcept {
        lv_indev_set_long_press_time(m_indev, ms);
        return *this;
    }

    /// Set long press repeat time
    Indev& long_press_repeat_time(uint32_t ms) noexcept {
        lv_indev_set_long_press_repeat_time(m_indev, ms);
        return *this;
    }

    /// Set scroll limit (pixels to move before scroll starts)
    Indev& scroll_limit(uint32_t px) noexcept {
        lv_indev_set_scroll_limit(m_indev, px);
        return *this;
    }

    /// Set scroll throw slowdown (higher = slower deceleration)
    Indev& scroll_throw(uint32_t slowdown) noexcept {
        lv_indev_set_scroll_throw(m_indev, slowdown);
        return *this;
    }
};

// ==================== Input Device Helpers ====================

/// Get active (currently processing) input device
inline lv_indev_t* indev_active() noexcept {
    return lv_indev_active();
}

/// Get next input device (for iteration)
inline lv_indev_t* indev_next(lv_indev_t* prev = nullptr) noexcept {
    return lv_indev_get_next(prev);
}

/// Enable or disable all input devices
inline void indev_enable_all(bool enable) noexcept {
    lv_indev_enable(nullptr, enable);
}

// ==================== Group (for keyboard navigation) ====================

/**
 * @brief Group wrapper for keyboard/encoder navigation
 */
class Group {
    lv_group_t* m_group;
    bool m_owned;

public:
    /// Create a new group
    Group() noexcept : m_group(lv_group_create()), m_owned(true) {}

    /// Wrap existing group (non-owning)
    explicit Group(lv_group_t* grp) noexcept : m_group(grp), m_owned(false) {}

    ~Group() {
        if (m_owned && m_group) {
            lv_group_delete(m_group);
        }
    }

    // Non-copyable
    Group(const Group&) = delete;
    Group& operator=(const Group&) = delete;

    // Moveable
    Group(Group&& other) noexcept : m_group(other.m_group), m_owned(other.m_owned) {
        other.m_group = nullptr;
        other.m_owned = false;
    }

    Group& operator=(Group&& other) noexcept {
        if (this != &other) {
            if (m_owned && m_group) {
                lv_group_delete(m_group);
            }
            m_group = other.m_group;
            m_owned = other.m_owned;
            other.m_group = nullptr;
            other.m_owned = false;
        }
        return *this;
    }

    /// Get underlying group pointer
    [[nodiscard]] lv_group_t* get() noexcept { return m_group; }
    [[nodiscard]] const lv_group_t* get() const noexcept { return m_group; }

    /// Implicit conversion
    operator lv_group_t*() noexcept { return m_group; }

    // ==================== Objects ====================

    /// Add object to group
    Group& add(ObjectView obj) noexcept {
        lv_group_add_obj(m_group, obj.get());
        return *this;
    }

    /// Remove object from group
    Group& remove(ObjectView obj) noexcept {
        lv_group_remove_obj(obj.get());
        return *this;
    }

    /// Remove all objects
    Group& remove_all() noexcept {
        lv_group_remove_all_objs(m_group);
        return *this;
    }

    /// Get focused object
    [[nodiscard]] ObjectView focused() const noexcept {
        return ObjectView(lv_group_get_focused(m_group));
    }

    // ==================== Focus ====================

    /// Focus next object
    Group& focus_next() noexcept {
        lv_group_focus_next(m_group);
        return *this;
    }

    /// Focus previous object
    Group& focus_prev() noexcept {
        lv_group_focus_prev(m_group);
        return *this;
    }

    /// Focus specific object
    Group& focus(ObjectView obj) noexcept {
        lv_group_focus_obj(obj.get());
        return *this;
    }

    /// Send key to focused object
    Group& send_key(uint32_t k) noexcept {
        lv_group_send_data(m_group, k);
        return *this;
    }

    // ==================== Wrap ====================

    /// Enable/disable wrap around
    Group& wrap(bool en) noexcept {
        lv_group_set_wrap(m_group, en);
        return *this;
    }

    /// Get wrap state
    [[nodiscard]] bool wrap() const noexcept {
        return lv_group_get_wrap(m_group);
    }

    // ==================== Editing ====================

    /// Set editing mode (for spinbox, etc.)
    Group& editing(bool en) noexcept {
        lv_group_set_editing(m_group, en);
        return *this;
    }

    /// Get editing state
    [[nodiscard]] bool editing() const noexcept {
        return lv_group_get_editing(m_group);
    }

    // ==================== Object Count ====================

    /// Get object count
    [[nodiscard]] uint32_t count() const noexcept {
        return lv_group_get_obj_count(m_group);
    }
};

/// Get default group
inline lv_group_t* group_default() noexcept {
    return lv_group_get_default();
}

/// Set default group
inline void group_set_default(lv_group_t* grp) noexcept {
    lv_group_set_default(grp);
}

} // namespace lv

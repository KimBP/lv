#pragma once

/**
 * @file focus.hpp
 * @brief Zero-cost wrapper for LVGL focus groups (keyboard/encoder navigation)
 */

#include <lvgl.h>
#include "object.hpp"

namespace lv {

/**
 * @brief Owning wrapper for lv_group_t (focus group)
 *
 * Use focus groups to navigate widgets with keyboard/encoder input.
 * The group is created/destroyed automatically (RAII) and is pointer-sized.
 *
 * @note On destruction, the group is automatically detached from any input
 * devices that reference it. This prevents dangling pointers in LVGL's
 * indev system. Cost: O(n) iteration over indevs (typically 1-3 devices).
 */
class FocusGroup {
    lv_group_t* m_group = nullptr;

    /// Detach this group from all input devices (if indev support is enabled)
    void detach_from_indevs() noexcept {
#if LV_USE_INDEV
        lv_indev_t* indev = lv_indev_get_next(nullptr);
        while (indev) {
            if (lv_indev_get_group(indev) == m_group) {
                lv_indev_set_group(indev, nullptr);
            }
            indev = lv_indev_get_next(indev);
        }
#endif
    }

public:
    /// Create a new focus group
    FocusGroup() : m_group(lv_group_create()) {}

    /// Take ownership of an existing focus group
    struct adopt_t {};
    static constexpr adopt_t adopt{};

    FocusGroup(adopt_t, lv_group_t* g) noexcept : m_group(g) {}

    /// Destructor detaches from indevs and deletes the group
    ~FocusGroup() {
        if (m_group) {
            detach_from_indevs();
            lv_group_del(m_group);
        }
    }

    // Non-copyable
    FocusGroup(const FocusGroup&) = delete;
    FocusGroup& operator=(const FocusGroup&) = delete;

    // Moveable
    FocusGroup(FocusGroup&& other) noexcept : m_group(other.m_group) {
        other.m_group = nullptr;
    }

    FocusGroup& operator=(FocusGroup&& other) noexcept {
        if (this != &other) {
            if (m_group) {
                // Clear any indev pointers before deleting current group
                detach_from_indevs();
                lv_group_del(m_group);
            }
            m_group = other.m_group;
            other.m_group = nullptr;
        }
        return *this;
    }

    /// Get raw group pointer
    [[nodiscard]] lv_group_t* get() const noexcept { return m_group; }
    [[nodiscard]] operator lv_group_t*() const noexcept { return m_group; }

    /// Add object to the focus group
    FocusGroup& add(ObjectView obj) noexcept {
        lv_group_add_obj(m_group, obj);
        return *this;
    }

    /// Remove object from the focus group
    FocusGroup& remove(ObjectView obj) noexcept {
        lv_group_remove_obj(obj);
        return *this;
    }

    /// Focus next object
    FocusGroup& focus_next() noexcept {
        lv_group_focus_next(m_group);
        return *this;
    }

    /// Focus previous object
    FocusGroup& focus_prev() noexcept {
        lv_group_focus_prev(m_group);
        return *this;
    }

    /// Focus a specific object
    FocusGroup& focus(ObjectView obj) noexcept {
        lv_group_focus_obj(obj);
        return *this;
    }

    /// Enable/disable focus wrap (loop)
    FocusGroup& wrap(bool enable = true) noexcept {
        lv_group_set_wrap(m_group, enable);
        return *this;
    }

    /// Enable/disable editing mode
    FocusGroup& edit_mode(bool enable = true) noexcept {
        lv_group_set_editing(m_group, enable);
        return *this;
    }
};

// Zero-cost check: wrapper must be pointer-sized
static_assert(sizeof(FocusGroup) == sizeof(void*),
    "FocusGroup must be pointer-sized");

} // namespace lv

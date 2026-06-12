#pragma once

/**
 * @file box.hpp
 * @brief Zero-cost wrapper for basic styled container object
 *
 * A Box is a simple styled container (lv_obj) with fluent API.
 * Use for custom-styled containers that aren't flex layouts.
 *
 * @code
 * // Create a box using the static factory
 * auto box = lv::Box::create(parent)
 *     .size(100, 50)
 *     .bg_color(lv::rgb(0x2196F3));  // blue
 *
 * // Wrap an existing lv_obj_t*
 * auto wrapped = lv::Box(lv::wrap, raw_ptr);
 * @endcode
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Basic styled container widget
 *
 * A simple object wrapper with full styling support.
 * Use when you need a custom-styled container without flex layout.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Box : public ObjectView,
            public ObjectMixin<Box>,
            public EventMixin<Box>,
            public StyleMixin<Box> {
public:
    /// Default constructor (null/invalid box)
    constexpr Box() noexcept : ObjectView(nullptr) {}

    /// Wrap an existing lv_obj_t* as a Box (does NOT create a new object)
    /// Usage: auto box = lv::Box(lv::wrap, existing_obj);
    constexpr Box(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    /// Constructor taking parent
    Box(ObjectView parent) noexcept 
    : ObjectView(lv_obj_create(parent.get()))
    {
      lv_obj_remove_flag(m_obj, LV_OBJ_FLAG_SCROLLABLE);
    }

    // Default copy/move - copies the pointer, does NOT create new lv_obj
    Box(const Box&) = default;
    Box& operator=(const Box&) = default;
    Box(Box&&) = default;
    Box& operator=(Box&&) = default;

    /**
     * @brief Create a new Box widget
     *
     * This is the preferred way to create widgets. It clearly indicates
     * that a new LVGL object is being created.
     *
     * @param parent Parent object (any ObjectView-derived type or lv_obj_t*)
     * @return New Box instance
     *
     * @code
     * auto box = lv::Box::create(parent)
     *     .size(100, 50)
     *     .bg_color(lv::rgb(0xF44336));  // red
     * @endcode
     */
    [[nodiscard]] static Box create(lv_obj_t* parent) {
        Box box;
        box.m_obj = lv_obj_create(parent);
        lv_obj_remove_flag(box.m_obj, LV_OBJ_FLAG_SCROLLABLE);
        return box;
    }

    [[nodiscard]] static Box create(ObjectView parent) {
        return create(parent.get());
    }

    // size(), width(), height(), fill(), fill_width(), fill_height(),
    // pos(), x(), y(), center(), align(), grow(), scrollable()
    // are inherited from ObjectMixin<Box>.
};

/**
 * @brief Owning Box — destroys the LVGL object on destruction.
 *
 * Identical to Box but owns the underlying lv_obj_t: the destructor calls
 * lv_obj_delete(), removing the widget and all its children from the tree.
 * Move-only to prevent double deletion.
 *
 * Use this when a C++ object must own a widget's lifetime, e.g. as a member
 * of a screen class where the widget must be destroyed before style members.
 *
 * @code
 * class MyScreen {
 *     lv::Style style;          // destroyed AFTER screen (reverse order)
 *     lv::OwnedBox screen;      // destroyed first — safe to hold style ptr
 * };
 * @endcode
 */
class OwnedBox : public ObjectView,
                 public ObjectMixin<OwnedBox>,
                 public EventMixin<OwnedBox>,
                 public StyleMixin<OwnedBox> {
public:
    /// Default constructor — null/empty (no LVGL object created)
    constexpr OwnedBox() noexcept : ObjectView(nullptr) {}

    /// Create a new LVGL object with the given parent and take ownership
    explicit OwnedBox(ObjectView parent) noexcept
    : ObjectView(lv_obj_create(parent.get()))
    {
        lv_obj_remove_flag(m_obj, LV_OBJ_FLAG_SCROLLABLE);
    }

    /// Destructor deletes the owned LVGL object
    ~OwnedBox() noexcept {
        if (m_obj) {
            lv_obj_delete(m_obj);
            m_obj = nullptr;
        }
    }

    // Non-copyable
    OwnedBox(const OwnedBox&) = delete;
    OwnedBox& operator=(const OwnedBox&) = delete;

    // Move-only
    OwnedBox(OwnedBox&& other) noexcept : ObjectView(other.m_obj) {
        other.m_obj = nullptr;
    }
    OwnedBox& operator=(OwnedBox&& other) noexcept {
        if (this != &other) {
            if (m_obj) lv_obj_delete(m_obj);
            m_obj = other.m_obj;
            other.m_obj = nullptr;
        }
        return *this;
    }

    /// Release ownership without deleting — caller takes responsibility
    [[nodiscard]] lv_obj_t* release() noexcept {
        auto* p = m_obj;
        m_obj = nullptr;
        return p;
    }
};

} // namespace lv

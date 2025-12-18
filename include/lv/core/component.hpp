#pragma once

/**
 * @file component.hpp
 * @brief Zero-overhead CRTP component base class
 *
 * Provides a React/SwiftUI-like component pattern using CRTP
 * (Curiously Recurring Template Pattern) for zero virtual call overhead.
 */

#include <lvgl.h>
#include "object.hpp"

namespace lv {

// ==================== Component User Data Wrapper ====================

/**
 * @brief Internal wrapper for component user_data to avoid collisions
 *
 * This allows components to use user_data while still allowing
 * user code to attach additional data.
 *
 * Contains a magic tag to verify the user_data actually points to
 * ComponentData before casting (prevents crashes if child widgets
 * set unrelated user_data).
 */
struct ComponentData {
    /// Magic value to identify ComponentData (0x4C56434D = "LVCM" in ASCII)
    static constexpr uint32_t MAGIC = 0x4C56434D;

    uint32_t magic;      ///< Magic tag for verification
    void* component;     ///< Pointer to Component instance
    void* user_payload;  ///< Optional user-defined payload

    constexpr ComponentData(void* comp) noexcept
        : magic(MAGIC), component(comp), user_payload(nullptr) {}

    /// Check if this is a valid ComponentData
    [[nodiscard]] static bool is_valid(void* ptr) noexcept {
        if (!ptr) return false;
        auto* data = static_cast<ComponentData*>(ptr);
        return data->magic == MAGIC;
    }
};

/**
 * @brief CRTP base class for UI components
 *
 * Components encapsulate UI logic and state. The derived class implements
 * build() which returns the root ObjectView of the component's UI tree.
 *
 * Features:
 * - Zero virtual call overhead (CRTP static dispatch)
 * - Automatic 'this' pointer storage in root's user_data (via ComponentData)
 * - Mount/unmount lifecycle management
 * - User payload support without colliding with component pointer
 *
 * Size: sizeof(void*) + sizeof(ComponentData)
 *       Typically 8 + 24 = 32 bytes on 64-bit (pointer + magic + component + payload)
 *
 * Usage:
 * @code
 * class MyComponent : public lv::Component<MyComponent> {
 *     lv::State<int> counter{0};
 *
 * public:
 *     lv::ObjectView build(lv::ObjectView parent) {
 *         auto root = lv::vbox(parent).padding(10).gap(5);
 *
 *         lv::Label(root)
 *             .bind_text(counter, "Count: %d");
 *
 *         lv::Button(root)
 *             .text("Increment")
 *             .on_click<&MyComponent::on_increment>(this);
 *
 *         return root;
 *     }
 *
 * private:
 *     void on_increment(lv_event_t*) {
 *         ++counter;
 *     }
 * };
 *
 * // Usage:
 * MyComponent comp;
 * comp.mount(lv::screen_active());
 * @endcode
 *
 * @tparam Derived The derived component class (CRTP pattern)
 */
template<typename Derived>
class Component {
protected:
    lv_obj_t* m_root = nullptr;
    ComponentData m_data{nullptr};  ///< Embedded data to avoid separate allocation

public:
    Component() = default;

    // Non-copyable (components manage UI state)
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;

    // Moveable
    Component(Component&& other) noexcept
        : m_root(other.m_root), m_data(other.m_data) {
        other.m_root = nullptr;
        // Update user_data to point to our embedded data
        if (m_root) {
            m_data.component = static_cast<Derived*>(this);
            lv_obj_set_user_data(m_root, &m_data);
        }
    }

    Component& operator=(Component&& other) noexcept {
        if (this != &other) {
            unmount();
            m_root = other.m_root;
            m_data = other.m_data;
            other.m_root = nullptr;
            if (m_root) {
                m_data.component = static_cast<Derived*>(this);
                lv_obj_set_user_data(m_root, &m_data);
            }
        }
        return *this;
    }

    /// Destructor unmounts if mounted
    ~Component() {
        unmount();
    }

    // ==================== Lifecycle ====================

    /**
     * @brief Mount the component to a parent
     *
     * Calls the derived class's build() method and stores the result.
     * The component's 'this' pointer is stored in the root's user_data
     * via ComponentData wrapper to avoid collisions with user payload.
     *
     * @param parent The parent object (ObjectView)
     */
    void mount(ObjectView parent) {
        if (m_root) {
            unmount();
        }

        // Call derived class build() - CRTP static dispatch
        ObjectView root = static_cast<Derived*>(this)->build(parent);
        m_root = root.get();

        if (m_root) {
            // Store component pointer in embedded data and attach to object
            m_data.component = static_cast<Derived*>(this);
            lv_obj_set_user_data(m_root, &m_data);

            // Call optional lifecycle hook
            if constexpr (requires { static_cast<Derived*>(this)->on_mount(); }) {
                static_cast<Derived*>(this)->on_mount();
            }
        }
    }

    /**
     * @brief Unmount the component
     *
     * Deletes the root object (LVGL cascades to children).
     */
    void unmount() {
        if (m_root) {
            // Call optional lifecycle hook
            if constexpr (requires { static_cast<Derived*>(this)->on_unmount(); }) {
                static_cast<Derived*>(this)->on_unmount();
            }

            lv_obj_delete(m_root);
            m_root = nullptr;
        }
    }

    /**
     * @brief Check if component is mounted
     */
    [[nodiscard]] bool is_mounted() const noexcept {
        return m_root != nullptr;
    }

    /**
     * @brief Check if component is visible
     */
    [[nodiscard]] bool is_visible() const noexcept {
        return m_root && !lv_obj_has_flag(m_root, LV_OBJ_FLAG_HIDDEN);
    }

    /**
     * @brief Hide the component (keeps UI in memory)
     */
    void hide() noexcept {
        if (m_root) {
            lv_obj_add_flag(m_root, LV_OBJ_FLAG_HIDDEN);
        }
    }

    /**
     * @brief Show the component
     */
    void show() noexcept {
        if (m_root) {
            lv_obj_remove_flag(m_root, LV_OBJ_FLAG_HIDDEN);
        }
    }

    /**
     * @brief Set visibility
     */
    void visible(bool v) noexcept {
        if (v) show(); else hide();
    }

    /**
     * @brief Get the root object as ObjectView
     */
    [[nodiscard]] ObjectView root() const noexcept {
        return ObjectView(m_root);
    }

    // ==================== User Payload (avoids user_data collision) ====================

    /**
     * @brief Set user-defined payload data
     *
     * This allows attaching custom data to the component without
     * conflicting with the internal component pointer storage.
     *
     * IMPORTANT: Always use this instead of root().user_data() on components!
     * Calling user_data() directly on the root object will overwrite the
     * ComponentData pointer and break from_event()/from_obj().
     *
     * @param payload Pointer to user data
     */
    void set_user_payload(void* payload) noexcept {
        m_data.user_payload = payload;
    }

    /**
     * @brief Get user-defined payload data
     */
    [[nodiscard]] void* get_user_payload() const noexcept {
        return m_data.user_payload;
    }

    /**
     * @brief Get user payload as typed pointer
     */
    template<typename T>
    [[nodiscard]] T* get_user_payload_as() const noexcept {
        return static_cast<T*>(m_data.user_payload);
    }

    /**
     * @brief Set payload on a child widget safely
     *
     * Use this to set user_data on child widgets within the component.
     * This is safe because only the root has ComponentData.
     *
     * @param child Child widget (must not be the root)
     * @param data User data pointer
     */
    void set_child_user_data(ObjectView child, void* data) noexcept {
        // Safety check: don't allow setting on root
        if (child.get() != m_root) {
            lv_obj_set_user_data(child.get(), data);
        }
    }

    /**
     * @brief Get payload from a child widget
     */
    template<typename T>
    [[nodiscard]] static T* get_child_user_data(ObjectView child) noexcept {
        return static_cast<T*>(lv_obj_get_user_data(child.get()));
    }

    // ==================== Static Helpers ====================

    /**
     * @brief Get component instance from an event
     *
     * Retrieves the component pointer from the root object's user_data.
     * Use this in event callbacks to get back to the component.
     * Walks up the widget tree to find the component root.
     *
     * @code
     * void handle_event(lv_event_t* e) {
     *     auto* self = MyComponent::from_event(e);
     *     if (self) self->do_something();
     * }
     * @endcode
     *
     * @return Pointer to component, or nullptr if not found or invalid
     */
    [[nodiscard]] static Derived* from_event(lv_event_t* e) noexcept {
        lv_obj_t* target = lv_event_get_current_target_obj(e);
        // Walk up to find root with valid ComponentData
        while (target) {
            void* data = lv_obj_get_user_data(target);
            if (ComponentData::is_valid(data)) {
                auto* comp_data = static_cast<ComponentData*>(data);
                return static_cast<Derived*>(comp_data->component);
            }
            target = lv_obj_get_parent(target);
        }
        return nullptr;
    }

    /**
     * @brief Get component instance from an object's user_data
     *
     * @return Pointer to component, or nullptr if user_data is not ComponentData
     */
    [[nodiscard]] static Derived* from_obj(ObjectView obj) noexcept {
        void* data = lv_obj_get_user_data(obj);
        if (ComponentData::is_valid(data)) {
            auto* comp_data = static_cast<ComponentData*>(data);
            return static_cast<Derived*>(comp_data->component);
        }
        return nullptr;
    }
};


/**
 * @brief Mixin for components that need screen integration
 *
 * Use this when the component represents a full screen.
 */
template<typename Derived>
class ScreenComponent : public Component<Derived> {
public:
    using Component<Derived>::Component;

    /**
     * @brief Mount as a new screen
     *
     * Creates a new screen object and mounts the component to it.
     */
    ObjectView mount_screen() {
        lv_obj_t* screen = lv_obj_create(nullptr);
        this->mount(ObjectView(screen));
        return ObjectView(screen);
    }

    /**
     * @brief Mount and load as active screen
     */
    void mount_and_load() {
        ObjectView screen = mount_screen();
        lv_screen_load(screen);
    }

    /**
     * @brief Mount and load with animation
     */
    void mount_and_load_anim(lv_screen_load_anim_t anim, uint32_t time,
                              uint32_t delay = 0, bool auto_del = true) {
        ObjectView screen = mount_screen();
        lv_screen_load_anim(screen, anim, time, delay, auto_del);
    }
};

} // namespace lv

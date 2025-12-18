#pragma once

/**
 * @file canvas.hpp
 * @brief Zero-cost wrapper for LVGL canvas widget
 */

#include <lvgl.h>
#include "../core/object.hpp"
#include "../core/event.hpp"
#include "../core/style.hpp"

namespace lv {

/**
 * @brief Canvas widget wrapper
 *
 * A drawable surface for custom graphics.
 *
 * Size: sizeof(void*) - 4 or 8 bytes
 */
class Canvas : public ObjectView,
            public ObjectMixin<Canvas>,
               public EventMixin<Canvas>,
               public StyleMixin<Canvas> {
public:
    constexpr Canvas() noexcept : ObjectView(nullptr) {}
    constexpr Canvas(wrap_t, lv_obj_t* obj) noexcept : ObjectView(obj) {}

    [[nodiscard]] static Canvas create(lv_obj_t* parent) {
        return Canvas(wrap, lv_canvas_create(parent));
    }
    [[nodiscard]] static Canvas create(ObjectView parent) {
        return create(parent.get());
    }

    // ==================== Buffer ====================

    /// Set buffer for canvas
    Canvas& buffer(void* buf, int32_t w, int32_t h, lv_color_format_t cf) noexcept {
        lv_canvas_set_buffer(m_obj, buf, w, h, cf);
        return *this;
    }

    /// Set draw buffer
    Canvas& draw_buf(lv_draw_buf_t* draw_buf) noexcept {
        lv_canvas_set_draw_buf(m_obj, draw_buf);
        return *this;
    }

    /// Get draw buffer
    [[nodiscard]] lv_draw_buf_t* draw_buf() const noexcept {
        return lv_canvas_get_draw_buf(m_obj);
    }

    // ==================== Pixel Operations ====================

    /// Set pixel color
    Canvas& set_px(int32_t x, int32_t y, lv_color_t color, lv_opa_t opa = LV_OPA_COVER) noexcept {
        lv_canvas_set_px(m_obj, x, y, color, opa);
        return *this;
    }

    // ==================== Drawing ====================

    /// Fill canvas with color
    Canvas& fill_bg(lv_color_t color, lv_opa_t opa = LV_OPA_COVER) noexcept {
        lv_canvas_fill_bg(m_obj, color, opa);
        return *this;
    }

    /// Initialize draw layer for drawing operations
    void init_layer(lv_layer_t* layer) noexcept {
        lv_canvas_init_layer(m_obj, layer);
    }

    /// Finish layer and render to canvas
    void finish_layer(lv_layer_t* layer) noexcept {
        lv_canvas_finish_layer(m_obj, layer);
    }

    // ==================== Copy ====================

    /// Copy from another draw buffer
    Canvas& copy_buf(const lv_area_t* canvas_area, lv_draw_buf_t* src_buf, const lv_area_t* src_area) noexcept {
        lv_canvas_copy_buf(m_obj, canvas_area, src_buf, src_area);
        return *this;
    }

    // ==================== Size ====================

    /// Set size
    Canvas& size(int32_t w, int32_t h) noexcept {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

} // namespace lv

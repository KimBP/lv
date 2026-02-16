/**
 * @file vector_graphics.cpp
 * @brief Vector graphics example demonstrating VectorPath and VectorDsc
 *
 * Shows how to draw SVG-like vector shapes using the LVGL vector graphics API.
 * Requires LV_USE_VECTOR_GRAPHIC enabled in lv_conf.h.
 */

#include <lv/lv.hpp>
#include <lv/draw/draw.hpp>
#include <lv/assets/cursor.hpp>
#include <cmath>

#if LV_USE_VECTOR_GRAPHIC

static void draw_vector_shapes(lv::Canvas& canvas) {
    // Create a layer for drawing
    lv::Layer layer;
    canvas.init_layer(layer);

    // === 1. Simple Triangle ===
    lv::VectorPath triangle;
    triangle.move_to(60, 30)
            .line_to(110, 120)
            .line_to(10, 120)
            .close();

    lv::VectorDsc dsc(layer.get());
    dsc.fill_color(lv::rgb(0x2196F3))      // Blue
       .fill_opa(LV_OPA_COVER)
       .stroke_color(lv::rgb(0x1565C0))    // Darker blue
       .stroke_width(2.0f)
       .add_path(triangle);

    // === 2. Rounded Rectangle ===
    lv::VectorPath rounded_rect;
    rounded_rect.rect(130, 30, 100, 90, 15, 15);

    dsc.fill_color(lv::rgb(0x4CAF50))      // Green
       .stroke_color(lv::rgb(0x2E7D32))    // Darker green
       .add_path(rounded_rect);

    // === 3. Circle ===
    lv::VectorPath circle_path;
    circle_path.circle(310, 75, 45);

    dsc.fill_color(lv::rgb(0xF44336))      // Red
       .stroke_color(lv::rgb(0xC62828))    // Darker red
       .add_path(circle_path);

    // === 4. Bezier Curve (Heart shape) ===
    lv::VectorPath heart;
    heart.move_to(60, 180)
         .cubic_to(60, 160, 30, 150, 30, 175)
         .cubic_to(30, 195, 60, 210, 60, 230)
         .cubic_to(60, 210, 90, 195, 90, 175)
         .cubic_to(90, 150, 60, 160, 60, 180)
         .close();

    dsc.fill_color(lv::rgb(0xE91E63))      // Pink
       .stroke_color(lv::rgb(0xAD1457))    // Darker pink
       .stroke_width(1.5f)
       .add_path(heart);

    // === 5. Star (using lines) ===
    lv::VectorPath star;
    float cx = 180, cy = 190;
    float outer_r = 40, inner_r = 18;
    star.move_to(cx, cy - outer_r);  // Top point

    for (int i = 1; i < 10; i++) {
        float angle = (i * 36.0f - 90.0f) * 3.14159f / 180.0f;
        float r = (i % 2 == 0) ? outer_r : inner_r;
        float x = cx + r * std::cos(angle);
        float y = cy + r * std::sin(angle);
        star.line_to(x, y);
    }
    star.close();

    dsc.fill_color(lv::rgb(0xFFC107))      // Amber
       .stroke_color(lv::rgb(0xFF9800))    // Orange
       .stroke_width(2.0f)
       .add_path(star);

    // === 6. Pie/Arc ===
    lv::VectorPath pie;
    pie.arc(310, 190, 45, 45, 270, true);  // pie = true

    dsc.fill_color(lv::rgb(0x9C27B0))      // Purple
       .stroke_color(lv::rgb(0x6A1B9A))    // Darker purple
       .add_path(pie);

    // === 7. Gradient Fill ===
    lv::VectorPath gradient_rect;
    gradient_rect.rect(10, 270, 110, 80, 8, 8);

    // Set up linear gradient
    lv_grad_stop_t stops[2];
    stops[0].frac = 0;
    stops[0].color = lv::rgb(0x4FC3F7);  // Light blue
    stops[0].opa = LV_OPA_COVER;
    stops[1].frac = 255;
    stops[1].color = lv::rgb(0x0D47A1);  // Dark blue
    stops[1].opa = LV_OPA_COVER;

    dsc.fill_linear_gradient(10, 270, 10, 350)
       .fill_gradient_stops(stops, 2)
       .stroke_opa(LV_OPA_TRANSP)  // No stroke
       .add_path(gradient_rect);

    // === 8. Dashed Stroke ===
    lv::VectorPath dashed_line;
    dashed_line.move_to(140, 310)
               .line_to(240, 310);

    float dash_pattern[] = {10.0f, 5.0f};  // 10px dash, 5px gap

    dsc.fill_opa(LV_OPA_TRANSP)
       .stroke_opa(LV_OPA_COVER)           // Reset stroke opacity!
       .stroke_color(lv::rgb(0x757575))    // Grey
       .stroke_width(3.0f)
       .stroke_dash(dash_pattern, 2)
       .stroke_cap(LV_VECTOR_STROKE_CAP_ROUND)
       .add_path(dashed_line);

    // === 9. Transformed Shape (rotated square) ===
    lv::VectorPath square;
    square.rect(-25, -25, 50, 50, 0, 0);

    dsc.identity()
       .translate(310, 310)
       .rotate(45)
       .fill_color(lv::rgb(0x009688))      // Teal
       .fill_opa(LV_OPA_COVER)             // Reset fill opacity!
       .stroke_color(lv::rgb(0x00695C))    // Darker teal
       .stroke_width(2.0f)
       .stroke_dash(nullptr, 0)  // Reset dash
       .add_path(square);

    // === 10. Complex Path (arrow) ===
    dsc.identity();  // Reset transform

    lv::VectorPath arrow;
    arrow.move_to(60, 400)
         .line_to(60, 430)
         .line_to(30, 430)
         .line_to(70, 470)
         .line_to(110, 430)
         .line_to(80, 430)
         .line_to(80, 400)
         .close();

    dsc.fill_color(lv::rgb(0x3F51B5))      // Indigo
       .fill_opa(LV_OPA_COVER)
       .stroke_color(lv::rgb(0x283593))    // Darker indigo
       .add_path(arrow);

    // Render all paths
    dsc.draw();

    // Finish the layer
    canvas.finish_layer(layer);
}

#endif // LV_USE_VECTOR_GRAPHIC

static void create_vector_demo(lv::ObjectView parent) {
    auto content = lv::vbox(parent)
        .fill()
        .padding(10)
        .gap(10);

    // Title
    lv::Label::create(content)
        .text("Vector Graphics Demo")
        .font(lv::fonts::montserrat_20)
        .text_color(lv::rgb(0x333333));

#if LV_USE_VECTOR_GRAPHIC
    // Status
    auto status = lv::Box::create(content)
        .fill_width()
        .height(LV_SIZE_CONTENT)
        .padding(8)
        .bg_color(lv::rgb(0xE8F5E9))
        .radius(8);

    lv::Label::create(status)
        .text("LV_USE_VECTOR_GRAPHIC: enabled")
        .text_color(lv::rgb(0x2E7D32));

    // Create canvas for vector drawing
    static lv::DrawBuf draw_buf(380, 500, lv::kColorFormat::ARGB8888);

    auto canvas = lv::Canvas::create(content)
        .size(380, 500);
    canvas.draw_buf(draw_buf.get());
    canvas.fill_bg(lv::rgb(0xF5F5F5));

    // Draw vector shapes
    draw_vector_shapes(canvas);

    // Labels for shapes
    auto labels = lv::Box::create(content)
        .fill_width()
        .height(LV_SIZE_CONTENT)
        .padding(8)
        .bg_color(lv::rgb(0xFFF3E0))
        .radius(8);

    lv::Label::create(labels)
        .text("1. Triangle  2. Rounded Rect  3. Circle\n"
              "4. Heart (Bezier)  5. Star  6. Pie\n"
              "7. Gradient  8. Dashed Line  9. Rotated\n"
              "10. Arrow")
        .text_color(lv::rgb(0xE65100));

#else
    // Disabled status
    auto status = lv::Box::create(content)
        .fill_width()
        .height(LV_SIZE_CONTENT)
        .padding(12)
        .bg_color(lv::rgb(0xFFEBEE))
        .radius(8);

    lv::Label::create(status)
        .text("LV_USE_VECTOR_GRAPHIC: disabled\n\n"
              "Enable in lv_conf.h:\n"
              "#define LV_USE_VECTOR_GRAPHIC 1\n"
              "#define LV_USE_MATRIX 1")
        .text_color(lv::rgb(0xC62828));
#endif
}

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("Vector Graphics", 400, 700, &lv::cursor_arrow);
#elif LV_USE_SDL
    lv::SDLDisplay display(400, 700);
#else
    #error "No display backend enabled"
#endif

    create_vector_demo(lv::screen_active());

    lv::run();
    return 0;
}

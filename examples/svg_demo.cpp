/**
 * @file svg_demo.cpp
 * @brief SVG rendering demo
 *
 * Demonstrates:
 * - Parsing SVG data into a DOM tree (lv::svg::Node)
 * - Drawing SVG onto a Canvas via init_layer/finish_layer
 * - Using RenderObj to query viewport size and memory usage
 *
 * Requires: LV_USE_SVG 1, LV_USE_VECTOR_GRAPHIC 1, LV_USE_CANVAS 1
 */

#include <lv/lv.hpp>
#include <lv/draw/draw.hpp>
#include <lv/assets/cursor.hpp>
#include <cstdio>

#if LV_USE_SVG && LV_USE_CANVAS

// A simple SVG document: colored circles and a rectangle
static constexpr const char* svg_data = R"(
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 200 200">
  <rect x="10" y="10" width="180" height="180" rx="20" ry="20"
        fill="#E3F2FD" stroke="#2196F3" stroke-width="3"/>
  <circle cx="70" cy="80" r="30" fill="#F44336" opacity="0.8"/>
  <circle cx="130" cy="80" r="30" fill="#4CAF50" opacity="0.8"/>
  <circle cx="100" cy="130" r="30" fill="#2196F3" opacity="0.8"/>
</svg>
)";

static constexpr const char* svg_star = R"(
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100">
  <polygon points="50,5 61,35 95,35 68,57 79,91 50,70 21,91 32,57 5,35 39,35"
           fill="#FFC107" stroke="#FF9800" stroke-width="2"/>
</svg>
)";

static void draw_svg_on_canvas(lv::Canvas& canvas, lv::svg::Node& doc) {
    lv::Layer layer;
    canvas.init_layer(layer);
    lv::svg::draw(layer.get(), doc);
    canvas.finish_layer(layer);
}

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("SVG Demo", 480, 480, &lv::cursor_arrow);
#elif LV_USE_SDL
    lv::SDLDisplay display(480, 480);
#else
    #error "No display backend enabled. Enable LV_USE_X11 or LV_USE_SDL in lv_conf.h"
#endif

    auto screen = lv::screen_active();
    screen.bg_color(lv::colors::white());

    auto layout = lv::vbox(screen)
        .fill()
        .padding(20)
        .gap(15);

    lv::Label::create(layout)
        .text("SVG Rendering Demo")
        .text_color(lv::rgb(0x2196F3));

    // ==================== SVG 1: Circles ====================

    lv::Label::create(layout).text("Circles (200x200 SVG on 220x220 canvas):");

    lv::svg::Node doc(svg_data);

    if (doc) {
        static lv::DrawBuf buf1(220, 220, lv::kColorFormat::ARGB8888);
        auto canvas = lv::Canvas::create(layout).size(220, 220);
        canvas.draw_buf(buf1.get());
        canvas.fill_bg(lv::rgb(0xF5F5F5));
        draw_svg_on_canvas(canvas, doc);
    }
    else {
        lv::Label::create(layout).text("Failed to parse SVG!");
    }

    // ==================== SVG 2: Star ====================

    lv::Label::create(layout).text("Star (100x100 SVG on 120x120 canvas):");

    lv::svg::Node star_doc(svg_star);

    if (star_doc) {
        static lv::DrawBuf buf2(120, 120, lv::kColorFormat::ARGB8888);
        auto canvas = lv::Canvas::create(layout).size(120, 120);
        canvas.draw_buf(buf2.get());
        canvas.fill_bg(lv::rgb(0xFFFDE7));
        draw_svg_on_canvas(canvas, star_doc);

        // Show RenderObj info
        lv::svg::RenderObj render(star_doc);
        if (render) {
            float w = 0, h = 0;
            if (render.viewport_size(w, h)) {
                lv::Label::create(layout)
                    .text_fmt("Viewport: %.0fx%.0f, Render size: %u bytes",
                              (double)w, (double)h, render.size());
            }
            else {
                lv::Label::create(layout)
                    .text_fmt("Render size: %u bytes", render.size());
            }
        }
    }

    lv::run();
    return 0;
}

#else

int main() {
    printf("This example requires LV_USE_SVG=1, LV_USE_VECTOR_GRAPHIC=1, and LV_USE_CANVAS=1 in lv_conf.h\n");
    return 1;
}

#endif

/**
 * @file texture3d_demo.cpp
 * @brief 3D Texture widget example
 *
 * Demonstrates the lv::Texture3D widget and lv::Draw3dDsc APIs.
 *
 * NOTE: This example shows the API usage but requires actual 3D backend
 * integration (OpenGL, Vulkan, etc.) to display real 3D textures.
 * Without a 3D backend, it shows placeholder content.
 *
 * For real usage, you would:
 * 1. Create an OpenGL texture with your 3D rendering
 * 2. Pass the GLuint texture ID to lv::Texture3D::src()
 * 3. LVGL will composite the texture into the UI
 */

#include <lv/lv.hpp>
#include <lv/assets/cursor.hpp>

#if LV_USE_3DTEXTURE
#include <lv/draw/draw_3d.hpp>
#endif

static void create_texture3d_demo(lv::ObjectView parent) {
    auto content = lv::vbox(parent)
        .fill()
        .padding(16)
        .gap(12);

    // Title
    lv::Label::create(content)
        .text("3D Texture API")
        .font(lv::fonts::montserrat_20)
        .text_color(lv::rgb(0x333333));

    // Status box
#if LV_USE_3DTEXTURE
    auto status_box = lv::Box::create(content)
        .fill_width()
        .height(LV_SIZE_CONTENT)
        .padding(8)
        .bg_color(lv::rgb(0xE8F5E9))
        .radius(8);

    lv::Label::create(status_box)
        .text("LV_USE_3DTEXTURE: enabled")
        .text_color(lv::rgb(0x2E7D32));
#else
    auto status_box = lv::Box::create(content)
        .fill_width()
        .height(LV_SIZE_CONTENT)
        .padding(8)
        .bg_color(lv::rgb(0xFFF3E0))
        .radius(8);

    lv::Label::create(status_box)
        .text("LV_USE_3DTEXTURE: disabled\n"
              "(requires LV_USE_OPENGLES)")
        .text_color(lv::rgb(0xE65100));
#endif

    // Section 1: Widget API
    lv::Label::create(content)
        .text("1. Texture3D Widget API:")
        .text_color(lv::rgb(0x666666));

    auto api1_box = lv::Box::create(content)
        .fill_width()
        .height(LV_SIZE_CONTENT)
        .padding(12)
        .bg_color(lv::rgb(0xE8F5E9))
        .radius(8);

    lv::Label::create(api1_box)
        .text("// Create widget\n"
              "lv::Texture3D::create(parent)\n"
              "    .size(400, 300)\n"
              "    .src(gl_texture_id)\n"
              "    .flip(false, true)  // V-flip for GL\n"
              "    .center();")
        .text_color(lv::rgb(0x1B5E20));

    // Section 2: Draw API
    lv::Label::create(content)
        .text("2. Low-level Draw API:")
        .text_color(lv::rgb(0x666666));

    auto api2_box = lv::Box::create(content)
        .fill_width()
        .height(LV_SIZE_CONTENT)
        .padding(12)
        .bg_color(lv::rgb(0xFFF3E0))
        .radius(8);

    lv::Label::create(api2_box)
        .text("// In draw event handler:\n"
              "lv::Draw3dDsc dsc;\n"
              "dsc.texture(tex_id)\n"
              "   .flip_v()\n"
              "   .opa(LV_OPA_COVER);\n"
              "lv::draw::texture_3d(layer, dsc, area);")
        .text_color(lv::rgb(0xE65100));

    // Section 3: Use cases
    lv::Label::create(content)
        .text("3. Use Cases:")
        .text_color(lv::rgb(0x666666));

    auto use_box = lv::Box::create(content)
        .fill_width()
        .height(LV_SIZE_CONTENT)
        .padding(12)
        .bg_color(lv::rgb(0xE3F2FD))
        .radius(8);

    lv::Label::create(use_box)
        .text("- 3D model viewer in UI\n"
              "- Game HUD overlay\n"
              "- Video playback\n"
              "- Camera preview\n"
              "- OpenGL/Vulkan integration")
        .text_color(lv::rgb(0x0D47A1));

    // Section 4: Requirements
    lv::Label::create(content)
        .text("4. Requirements:")
        .text_color(lv::rgb(0x666666));

    auto req_box = lv::Box::create(content)
        .fill_width()
        .height(LV_SIZE_CONTENT)
        .padding(12)
        .bg_color(lv::rgb(0xFCE4EC))
        .radius(8);

    lv::Label::create(req_box)
        .text("In lv_conf.h:\n"
              "#define LV_USE_OPENGLES   1\n"
              "#define LV_USE_3DTEXTURE  1\n\n"
              "Link with OpenGL libraries")
        .text_color(lv::rgb(0xC2185B));
}

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("3D Texture Demo", 400, 720, &lv::cursor_arrow);
#elif LV_USE_SDL
    lv::SDLDisplay display(400, 720);
#else
    #error "No display backend enabled"
#endif

    create_texture3d_demo(lv::screen_active());

    lv::run();
    return 0;
}

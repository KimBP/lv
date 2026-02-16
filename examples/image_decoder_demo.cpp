/**
 * @file image_decoder_demo.cpp
 * @brief Image decoder example demonstrating LVGL image decoding API
 *
 * Shows:
 * - Getting image info without decoding
 * - Decoding images with ImageDecoderDsc
 * - Displaying decoded image dimensions
 */

#include <lv/lv.hpp>
#include <lv/draw/image_decoder.hpp>
#include <lv/assets/cursor.hpp>

// Sample image - a small 4x4 red square (embedded C array)
static const uint8_t img_red_square_data[4 * 4 * 4] = {
    // 4x4 ARGB8888 red pixels
    0xFF,0x00,0x00,0xFF, 0xFF,0x00,0x00,0xFF, 0xFF,0x00,0x00,0xFF, 0xFF,0x00,0x00,0xFF,
    0xFF,0x00,0x00,0xFF, 0xFF,0x00,0x00,0xFF, 0xFF,0x00,0x00,0xFF, 0xFF,0x00,0x00,0xFF,
    0xFF,0x00,0x00,0xFF, 0xFF,0x00,0x00,0xFF, 0xFF,0x00,0x00,0xFF, 0xFF,0x00,0x00,0xFF,
    0xFF,0x00,0x00,0xFF, 0xFF,0x00,0x00,0xFF, 0xFF,0x00,0x00,0xFF, 0xFF,0x00,0x00,0xFF,
};

static const lv_image_dsc_t img_red_square = {
    .header = {
        .magic = LV_IMAGE_HEADER_MAGIC,
        .cf = LV_COLOR_FORMAT_ARGB8888,
        .flags = 0,
        .w = 4,
        .h = 4,
        .stride = 4 * 4,  // width * bytes_per_pixel
        .reserved_2 = 0,
    },
    .data_size = sizeof(img_red_square_data),
    .data = img_red_square_data,
    .reserved = nullptr,
    .reserved_2 = nullptr,
};

static void create_decoder_demo(lv::ObjectView parent) {
    auto content = lv::vbox(parent)
        .fill()
        .padding(16)
        .gap(12);

    // Title
    lv::Label::create(content)
        .text("Image Decoder Demo")
        .font(lv::fonts::montserrat_20)
        .text_color(lv::rgb(0x333333));

    // Section 1: Get image info
    lv::Label::create(content)
        .text("1. Get image info (no decode):")
        .text_color(lv::rgb(0x666666));

    auto info_box = lv::Box::create(content)
        .fill_width()
        .height(LV_SIZE_CONTENT)
        .padding(12)
        .bg_color(lv::rgb(0xF0F0F0))
        .radius(8);

    // Get info from embedded image
    lv_image_header_t header{};
    lv_result_t res = lv::image_get_info(&img_red_square, header);

    static char info_text[128];
    if (res == LV_RESULT_OK) {
        lv::snprintf(info_text, sizeof(info_text),
            "Image: %dx%d\nFormat: %d\nResult: OK",
            header.w, header.h, header.cf);
    } else {
        lv::snprintf(info_text, sizeof(info_text), "Failed to get image info");
    }

    lv::Label::create(info_box)
        .text(info_text)
        .text_color(lv::rgb(0x333333));

    // Section 2: Decode session demo
    lv::Label::create(content)
        .text("2. Decode session (RAII):")
        .text_color(lv::rgb(0x666666));

    auto decode_box = lv::Box::create(content)
        .fill_width()
        .height(LV_SIZE_CONTENT)
        .padding(12)
        .bg_color(lv::rgb(0xE8F5E9))
        .radius(8);

    // Use RAII decoder session
    lv::ImageDecoderDsc dsc;
    static char decode_text[256];

    if (dsc.open(&img_red_square)) {
        lv::snprintf(decode_text, sizeof(decode_text),
            "Decoded: %dx%d\n"
            "Format: %d\n"
            "Buffer: %s\n"
            "Session auto-closes on scope exit",
            dsc.width(), dsc.height(),
            dsc.color_format(),
            dsc.decoded() ? "valid" : "null");
    } else {
        lv::snprintf(decode_text, sizeof(decode_text),
            "Decode failed: %s",
            dsc.error_msg() ? dsc.error_msg() : "unknown error");
    }

    lv::Label::create(decode_box)
        .text(decode_text)
        .text_color(lv::rgb(0x1B5E20));

    // Section 3: API overview
    lv::Label::create(content)
        .text("3. API Overview:")
        .text_color(lv::rgb(0x666666));

    auto api_box = lv::Box::create(content)
        .fill_width()
        .height(LV_SIZE_CONTENT)
        .padding(12)
        .bg_color(lv::rgb(0xFFF3E0))
        .radius(8);

    lv::Label::create(api_box)
        .text("lv::ImageDecoderDsc dsc;\n"
              "dsc.open(src);      // Open image\n"
              "dsc.width();        // Get dimensions\n"
              "dsc.decoded();      // Get pixel buffer\n"
              "// Auto-closes on destruction")
        .text_color(lv::rgb(0xE65100));

    // Section 4: Custom decoder info
    lv::Label::create(content)
        .text("4. Custom Decoder:")
        .text_color(lv::rgb(0x666666));

    auto custom_box = lv::Box::create(content)
        .fill_width()
        .height(LV_SIZE_CONTENT)
        .padding(12)
        .bg_color(lv::rgb(0xE3F2FD))
        .radius(8);

    lv::Label::create(custom_box)
        .text("lv::ImageDecoder decoder;\n"
              "decoder.create();\n"
              "decoder.set_info_cb(...);\n"
              "decoder.set_open_cb(...);\n"
              "// Auto-deleted on destruction")
        .text_color(lv::rgb(0x0D47A1));
}

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("Image Decoder Demo", 400, 600, &lv::cursor_arrow);
#elif LV_USE_SDL
    lv::SDLDisplay display(400, 600);
#else
    #error "No display backend enabled"
#endif

    create_decoder_demo(lv::screen_active());

    lv::run();
    return 0;
}

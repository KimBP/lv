/**
 * @file lottie_demo.cpp
 * @brief Lottie animation example demonstrating lv::Lottie wrapper
 *
 * This example shows:
 * - Loading Lottie animations using ThorVG
 * - Animation playback controls (pause/resume)
 * - Using C++ wrapper fluent API
 *
 * Build:
 *   cmake -B build
 *   cmake --build build
 *   ./build/examples/lottie_demo
 *
 * Note: Requires LV_USE_LOTTIE and LV_USE_THORVG enabled in lv_conf.h
 */

#include <lv/lv.hpp>

#if LV_USE_LOTTIE

// Simple checkmark animation JSON (embedded)
// This is a minimal Lottie animation for demonstration
static const char lottie_checkmark_json[] = R"({
  "v": "5.5.2",
  "fr": 60,
  "ip": 0,
  "op": 60,
  "w": 200,
  "h": 200,
  "nm": "Checkmark",
  "ddd": 0,
  "assets": [],
  "layers": [
    {
      "ddd": 0,
      "ind": 1,
      "ty": 4,
      "nm": "Check",
      "sr": 1,
      "ks": {
        "o": {"a": 0, "k": 100},
        "r": {"a": 0, "k": 0},
        "p": {"a": 0, "k": [100, 100, 0]},
        "a": {"a": 0, "k": [0, 0, 0]},
        "s": {"a": 0, "k": [100, 100, 100]}
      },
      "ao": 0,
      "shapes": [
        {
          "ty": "gr",
          "it": [
            {
              "ind": 0,
              "ty": "sh",
              "ks": {
                "a": 0,
                "k": {
                  "i": [[0, 0], [0, 0], [0, 0]],
                  "o": [[0, 0], [0, 0], [0, 0]],
                  "v": [[-40, 0], [-15, 25], [40, -30]],
                  "c": false
                }
              },
              "nm": "Path"
            },
            {
              "ty": "st",
              "c": {"a": 0, "k": [0.2, 0.7, 0.3, 1]},
              "o": {"a": 0, "k": 100},
              "w": {"a": 0, "k": 12},
              "lc": 2,
              "lj": 2,
              "nm": "Stroke"
            },
            {
              "ty": "tm",
              "s": {"a": 0, "k": 0},
              "e": {
                "a": 1,
                "k": [
                  {"t": 0, "s": [0], "i": {"x": [0.4], "y": [1]}, "o": {"x": [0.6], "y": [0]}},
                  {"t": 40, "s": [100]}
                ]
              },
              "o": {"a": 0, "k": 0},
              "m": 1,
              "nm": "Trim"
            },
            {
              "ty": "tr",
              "p": {"a": 0, "k": [0, 0]},
              "a": {"a": 0, "k": [0, 0]},
              "s": {"a": 0, "k": [100, 100]},
              "r": {"a": 0, "k": 0},
              "o": {"a": 0, "k": 100}
            }
          ],
          "nm": "Group",
          "np": 4,
          "cix": 2,
          "bm": 0
        }
      ],
      "ip": 0,
      "op": 60,
      "st": 0,
      "bm": 0
    },
    {
      "ddd": 0,
      "ind": 2,
      "ty": 4,
      "nm": "Circle",
      "sr": 1,
      "ks": {
        "o": {"a": 0, "k": 100},
        "r": {"a": 0, "k": 0},
        "p": {"a": 0, "k": [100, 100, 0]},
        "a": {"a": 0, "k": [0, 0, 0]},
        "s": {
          "a": 1,
          "k": [
            {"t": 0, "s": [0, 0, 100], "i": {"x": [0.4, 0.4, 0.4], "y": [1, 1, 1]}, "o": {"x": [0.6, 0.6, 0.6], "y": [0, 0, 0]}},
            {"t": 30, "s": [100, 100, 100]}
          ]
        }
      },
      "ao": 0,
      "shapes": [
        {
          "ty": "gr",
          "it": [
            {
              "d": 1,
              "ty": "el",
              "s": {"a": 0, "k": [160, 160]},
              "p": {"a": 0, "k": [0, 0]},
              "nm": "Ellipse"
            },
            {
              "ty": "st",
              "c": {"a": 0, "k": [0.2, 0.7, 0.3, 1]},
              "o": {"a": 0, "k": 100},
              "w": {"a": 0, "k": 8},
              "lc": 2,
              "lj": 1,
              "nm": "Stroke"
            },
            {
              "ty": "tr",
              "p": {"a": 0, "k": [0, 0]},
              "a": {"a": 0, "k": [0, 0]},
              "s": {"a": 0, "k": [100, 100]},
              "r": {"a": 0, "k": 0},
              "o": {"a": 0, "k": 100}
            }
          ],
          "nm": "Circle Group",
          "np": 3,
          "cix": 2,
          "bm": 0
        }
      ],
      "ip": 0,
      "op": 60,
      "st": 0,
      "bm": 0
    }
  ]
})";

class LottieDemo : public lv::Component<LottieDemo> {
    // Animation buffer (ARGB8888 format: width * height * 4 bytes)
    static constexpr int ANIM_SIZE = 128;
    uint8_t m_anim_buf[ANIM_SIZE * ANIM_SIZE * 4];

    lv::Lottie m_lottie;
    bool m_paused = false;

public:
    lv::ObjectView build(lv::ObjectView parent) {
        auto root = lv::vbox(parent)
            .fill()
            .padding(20)
            .gap(16)
            .center_content();

        // Title
        lv::Label::create(root)
            .text("Lottie Animation Demo")
            .font(lv::fonts::montserrat_20)
            .center_text();

        // Description
        lv::Label::create(root)
            .text("ThorVG-powered vector animation")
            .center_text();

        // Lottie animation widget
        m_lottie = lv::Lottie::create(root)
            .buffer(ANIM_SIZE, ANIM_SIZE, m_anim_buf)
            .src_data(lottie_checkmark_json, sizeof(lottie_checkmark_json) - 1)
            .loop();

        // Control buttons
        auto buttons = lv::hbox(root)
            .gap(16)
            .size_content();

        lv::Button::create(buttons)
            .size(100, 50)
            .radius(8)
            .text("Pause")
            .on_click<&LottieDemo::toggle_pause>(this);

        lv::Button::create(buttons)
            .size(100, 50)
            .radius(8)
            .text("Restart")
            .on_click<&LottieDemo::restart>(this);

        return root;
    }

private:
    void toggle_pause(lv::Event e) {
        m_paused = !m_paused;
        if (m_paused) {
            m_lottie.pause();
            // Update button text
            auto btn = e.target();
            lv::Label(lv::wrap, btn.child(0).get()).text("Resume");
        } else {
            m_lottie.resume();
            auto btn = e.target();
            lv::Label(lv::wrap, btn.child(0).get()).text("Pause");
        }
    }

    void restart(lv::Event) {
        // Restart animation by re-setting the source
        m_lottie.src_data(lottie_checkmark_json, sizeof(lottie_checkmark_json) - 1);
        m_paused = false;
    }
};

#endif // LV_USE_LOTTIE

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("Lottie Demo", 800, 480);
#elif LV_USE_SDL
    lv::SDLDisplay display(800, 480);
#else
    #error "No display backend enabled"
#endif

#if LV_USE_LOTTIE
    LottieDemo app;
    app.mount(lv::screen_active());
#else
    lv::Label::create(lv::screen_active())
        .text("Enable LV_USE_LOTTIE and LV_USE_THORVG\nin lv_conf.h for this demo")
        .center();
#endif

    lv::run();
}

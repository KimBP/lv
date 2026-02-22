/**
 * @file fragment_demo.cpp
 * @brief Fragment lifecycle and navigation stack demo
 *
 * Demonstrates:
 * - Defining fragment classes with lv_fragment_class_t
 * - FragmentManager for push/pop navigation
 * - Fragment object creation and destruction lifecycle
 * - Back navigation with stack
 *
 * Requires: LV_USE_FRAGMENT 1
 */

#include <lv/lv.hpp>
#include <lv/assets/cursor.hpp>
#include <cstdio>

#if LV_USE_FRAGMENT

// ==================== Fragment Definitions ====================

// Each fragment struct extends lv_fragment_t with custom data.
// instance_size tells LVGL how much to allocate.
// (These structs and vtables are inherently C — the fragment system
// uses a C vtable pattern that cannot be wrapped further.)

struct HomeFragment {
    lv_fragment_t base;
};

struct DetailFragment {
    lv_fragment_t base;
    const char* title;
};

// Forward declarations
static lv_obj_t* home_create_obj(lv_fragment_t* self, lv_obj_t* container);
static lv_obj_t* detail_create_obj(lv_fragment_t* self, lv_obj_t* container);
static void detail_constructor(lv_fragment_t* self, void* args);

static const lv_fragment_class_t home_cls = {
    .constructor_cb = nullptr,
    .destructor_cb = nullptr,
    .attached_cb = nullptr,
    .detached_cb = nullptr,
    .create_obj_cb = home_create_obj,
    .obj_created_cb = nullptr,
    .obj_will_delete_cb = nullptr,
    .obj_deleted_cb = nullptr,
    .event_cb = nullptr,
    .instance_size = sizeof(HomeFragment),
};

static const lv_fragment_class_t detail_cls = {
    .constructor_cb = detail_constructor,
    .destructor_cb = nullptr,
    .attached_cb = nullptr,
    .detached_cb = nullptr,
    .create_obj_cb = detail_create_obj,
    .obj_created_cb = nullptr,
    .obj_will_delete_cb = nullptr,
    .obj_deleted_cb = nullptr,
    .event_cb = nullptr,
    .instance_size = sizeof(DetailFragment),
};

// ==================== Global State ====================

static lv::FragmentManager* g_mgr = nullptr;
static lv_obj_t* g_container = nullptr;

// Navigation action — member function pattern for stateful click handling
struct NavAction {
    const char* page;
    void on_click() {
        auto* frag = lv::fragment::create(&detail_cls, const_cast<char*>(page));
        g_mgr->push(frag, &g_container);
    }
};

static NavAction nav_actions[] = {{"Settings"}, {"Profile"}, {"About"}};

// ==================== Fragment Implementations ====================

static lv_obj_t* home_create_obj(lv_fragment_t* /*self*/, lv_obj_t* container) {
    auto root = lv::vbox(lv::ObjectView(container))
        .fill()
        .padding(20)
        .gap(15)
        .bg_color(lv::colors::white());

    lv::Label::create(root)
        .text("Fragment Demo")
        .text_color(lv::rgb(0x2196F3));

    lv::Label::create(root)
        .text("Each page is a Fragment managed\nby FragmentManager's nav stack.");

    // Navigation buttons — member function trampoline for per-button state
    for (auto& action : nav_actions) {
        auto btn = lv::Button::create(root).fill_width();
        btn.on<&NavAction::on_click>(lv::kEvent::clicked, &action);
        lv::Label::create(btn).text(action.page);
    }

    lv::Label::create(root)
        .text_fmt("Stack depth: %zu", g_mgr->stack_size());

    return root;
}

static void detail_constructor(lv_fragment_t* self, void* args) {
    auto* frag = reinterpret_cast<DetailFragment*>(self);
    frag->title = static_cast<const char*>(args);
}

static lv_obj_t* detail_create_obj(lv_fragment_t* self, lv_obj_t* container) {
    auto* frag = reinterpret_cast<DetailFragment*>(self);

    auto root = lv::vbox(lv::ObjectView(container))
        .fill()
        .padding(20)
        .gap(15)
        .bg_color(lv::rgb(0xf5f5f5));

    // Header with back button
    auto header = lv::hbox(root)
        .fill_width()
        .gap(10)
        .align_items(lv::kFlexAlign::center);

    lv::Button::create(header)
        .text(lv::symbol::left)
        .on_click([](lv::Event) { g_mgr->pop(); });

    lv::Label::create(header)
        .text(frag->title)
        .text_color(lv::rgb(0x2196F3))
        .grow(1);

    // Content
    auto card = lv::vbox(root)
        .fill_width()
        .padding(20)
        .gap(10)
        .bg_color(lv::colors::white())
        .radius(12);

    lv::Label::create(card)
        .text_fmt("This is the %s fragment.", frag->title);

    lv::Label::create(card)
        .text_fmt("Stack depth: %zu", g_mgr->stack_size());

    lv::Label::create(card)
        .text("Press back to pop this fragment\nfrom the navigation stack.");

    return root;
}

// ==================== Main ====================

int main() {
    lv::init();

#if LV_USE_X11
    lv::X11Display display("Fragment Demo", 480, 480, &lv::cursor_arrow);
#elif LV_USE_SDL
    lv::SDLDisplay display(480, 480);
#else
    #error "No display backend enabled. Enable LV_USE_X11 or LV_USE_SDL in lv_conf.h"
#endif

    auto screen = lv::screen_active();
    g_container = screen.get();

    lv::FragmentManager mgr;
    g_mgr = &mgr;

    auto* home = lv::fragment::create(&home_cls);
    mgr.push(home, &g_container);

    lv::run();
    return 0;
}

#else

int main() {
    printf("This example requires LV_USE_FRAGMENT=1 in lv_conf.h\n");
    return 1;
}

#endif // LV_USE_FRAGMENT

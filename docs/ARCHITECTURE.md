# lv:: C++20 Bindings for LVGL - Architecture Document

## Overview

This library provides **zero-cost C++20 abstractions** over LVGL's C API, enabling modern, type-safe, declarative UI development while maintaining full compatibility with LVGL's retained-mode widget system.

The design philosophy is: **C++ is the only UI language** - no DSL, no XML, no code generation, no moc. Just pure C++20 with CMake.

---

## Core Design Principles

### 1. Zero-Cost Abstractions

Widget wrapper classes are exactly `sizeof(void*)` - a single pointer to the underlying `lv_obj_t*`. RAII utility types have minimal overhead: `Style` embeds `lv_style_t` directly, `Timer` stores a pointer plus an ownership flag. There is no runtime overhead compared to using the C API directly.

```cpp
static_assert(sizeof(lv::Button) == sizeof(void*));
static_assert(sizeof(lv::Label) == sizeof(void*));
```

The compiler inlines all wrapper methods, producing identical assembly to hand-written C code.

### 2. Fluent Builder API

All widget configuration uses method chaining for readable, declarative UI code:

```cpp
lv::Button::create(parent)
    .text("Click me")
    .size(120, 40)
    .bg_color(lv::rgb(0x2196F3))  // blue
    .on_click<&MyClass::handle_click>(this);
```

### 3. Type-Safe Event Handling

Events use compile-time member function pointers instead of string-based bindings:

```cpp
// Type-safe: compiler verifies method signature
button.on_click<&MyApp::on_button_clicked>(this);

// Also supports stateless lambdas (no captures, zero allocation)
button.on_click([](lv::Event e) {
    // handle click
});
```

This is **refactor-safe** - renaming a method updates all references via the compiler.

### 4. RAII Where Appropriate

- `lv::Object` - Owning wrapper, deletes widget in destructor
- `lv::ObjectView` - Non-owning view (default for most widgets)
- `lv::Timer` - RAII timer management
- `lv::Style` - RAII style management
- `lv::DrawBuf` - RAII draw buffer for Canvas

Most widgets use non-owning semantics because LVGL's parent-child hierarchy manages lifetime.

### 5. No Hidden Allocations

The library avoids heap allocations in the wrapper layer. State management (`lv::State<T>`) embeds the LVGL subject directly, avoiding indirection.

---

## Architecture Layers

```
┌─────────────────────────────────────────────────────────┐
│                    Application Code                     │
│         (Components, ViewModels, Business Logic)        │
├─────────────────────────────────────────────────────────┤
│                   lv:: C++ Wrappers                     │
│    ┌─────────────┬─────────────┬─────────────────────┐  │
│    │   Widgets   │   Layouts   │   Core Services     │  │
│    │  (37 types) │ (Flex/Grid) │ (Event/State/Timer) │  │
│    └─────────────┴─────────────┴─────────────────────┘  │
├─────────────────────────────────────────────────────────┤
│                      LVGL C API                         │
│              (Retained-mode widget system)              │
├─────────────────────────────────────────────────────────┤
│                   Display Backend                       │
│               (SDL / X11 / Framebuffer)                 │
└─────────────────────────────────────────────────────────┘
```

---

## Module Breakdown

### Core (`include/lv/core/`)

| File | Purpose |
|------|---------|
| `object.hpp` | Base `ObjectView`/`Object` classes + global constants (`kState`, `kPart`, `kFlag`, `kDirection`, `kAlign`, etc.) |
| `event.hpp` | Type-safe event handling with `EventMixin<Derived>` CRTP |
| `style.hpp` | Style management with `StyleMixin<Derived>` CRTP |
| `color.hpp` | Color utilities (`hex()`, `rgb()`, `colors::` namespace) |
| `font.hpp` | Font handling, `DynamicFont` for runtime TTF loading |
| `timer.hpp` | RAII `Timer` wrapper |
| `anim.hpp` | Animation system with path callbacks |
| `anim_timeline.hpp` | RAII `AnimTimeline` for sequencing animations |
| `screen.hpp` | Screen management, `Navigator` for screen stack |
| `state.hpp` | Reactive `State<T>` using LVGL observer system |
| `component.hpp` | CRTP `Component<Derived>` base for custom widgets |
| `string_utils.hpp` | String utilities including `lv::snprintf()` |
| `image.hpp` | Image handling utilities |
| `indev.hpp` | Input device wrappers |
| `focus.hpp` | Focus group (`lv_group_t`) RAII wrapper for keyboard/encoder navigation |
| `fs.hpp` | RAII `fs::File` and `fs::Directory` for LVGL filesystem |
| `snapshot.hpp` | Object-to-image capture (requires `LV_USE_SNAPSHOT`) |
| `gridnav.hpp` | Arrow-key grid navigation (requires `LV_USE_GRIDNAV`) |
| `theme.hpp` | Theme application |
| `translation.hpp` | i18n support |

### Widgets (`include/lv/widgets/`)

37 widget wrappers, each following the same pattern:

```cpp
class WidgetName : public ObjectView,
                   public ObjectMixin<WidgetName>,
                   public EventMixin<WidgetName>,
                   public StyleMixin<WidgetName> {
public:
    [[nodiscard]] static WidgetName create(ObjectView parent);

    // Widget-specific configuration methods
    WidgetName& property(value) noexcept;

    // Getters where needed
    [[nodiscard]] Type property() const noexcept;
};
```

**Wrapped widgets**: Box (always available), Label, Button, Switch, Slider, Checkbox, Dropdown, Roller, Textarea, Spinbox, Arc, Bar, Chart, Table, List, Menu, Tabview, Tileview, Calendar, Keyboard, ButtonMatrix, Canvas, Led, Line, Spinner, Scale, Spangroup, Window, Msgbox, ImageButton, AnimImage, Image, ArcLabel, IMEPinyin, Texture3D, Lottie, FileExplorer (conditional on respective `LV_USE_*`)

### Layouts (`include/lv/layout/`)

| File | Purpose |
|------|---------|
| `flex.hpp` | Flexbox layout (`hbox()`, `vbox()`) with gap, alignment, grow |
| `grid.hpp` | CSS Grid layout with `fr()` units, spanning, alignment |

### Display (`include/lv/core/display.hpp`)

All display backends are in a single file:

| Class | Purpose |
|-------|---------|
| `Display` | Base display interface |
| `X11Display` | X11 backend for Linux desktop |
| `SDLDisplay` | SDL2 backend for cross-platform |
| `FBDisplay` | Framebuffer backend for embedded |
| `DRMDisplay` | DRM/KMS backend for embedded Linux |

### Draw API (`include/lv/draw/`)

Low-level drawing API for Canvas widget and custom graphics.

| File | Purpose |
|------|---------|
| `draw.hpp` | Umbrella header for all draw types |
| `draw_buf.hpp` | RAII `DrawBuf` for canvas buffers |
| `layer.hpp` | `Layer` wrapper for draw operations |
| `primitives.hpp` | Helper functions for `lv_area_t`, `lv_point_t` |
| `draw_rect.hpp` | `FillDsc`, `BorderDsc`, `BoxShadowDsc`, `RectDsc` |
| `draw_line.hpp` | `LineDsc` for line drawing |
| `draw_arc.hpp` | `ArcDsc` for arc drawing |
| `draw_triangle.hpp` | `TriangleDsc` for triangle drawing |
| `draw_label.hpp` | `LabelDsc`, `LetterDsc` for text |
| `draw_image.hpp` | `ImageDsc` for image drawing |

**Example**:
```cpp
lv::DrawBuf buf(200, 200, lv::kColorFormat::ARGB8888);
canvas.draw_buf(buf.get());

lv::Layer layer;
canvas.init_layer(layer);
lv::draw::rect(layer, rect_dsc, lv::area(10, 10, 100, 100));
lv::draw::line(layer, line_dsc);
canvas.finish_layer(layer);
```

---

## Constants and Type System

### Global Constants (k* Namespaces)

All LVGL `LV_*` constants are wrapped in `k`-prefixed namespaces with snake_case values:

```cpp
// Object states (for styling)
lv::kState::checked, lv::kState::pressed, lv::kState::disabled

// Object parts (for styling specific areas)
lv::kPart::main, lv::kPart::indicator, lv::kPart::knob

// Object flags (behavior modifiers)
lv::kFlag::scrollable, lv::kFlag::clickable, lv::kFlag::hidden

// Direction
lv::kDirection::left, lv::kDirection::horizontal, lv::kDirection::all

// Alignment
lv::kAlign::center, lv::kAlign::top_left, lv::kAlign::bottom_right

// Text alignment
lv::kTextAlign::left, lv::kTextAlign::center, lv::kTextAlign::right

// And more: kBorderSide, kGradientDirection, kBlendMode, kBaseDirection, kScrollSnap
```

### Widget-Specific Constants (Nested)

Constants specific to a widget are nested inside the class:

```cpp
// Chart
lv::Chart::Type::line, lv::Chart::Type::bar, lv::Chart::Type::scatter
lv::Chart::Axis::primary_y, lv::Chart::Axis::secondary_y
lv::Chart::UpdateMode::shift, lv::Chart::UpdateMode::circular

// Grid
lv::Grid::fr(1)  // fractional unit
lv::Grid::content
lv::Grid::template_last
lv::Grid::Align::start, lv::Grid::Align::center
```

### Type Aliases (Value Types Only)

Type aliases are used for value types, NOT for pointers:

```cpp
// Good - value types
using Color = lv_color_t;
using Opacity = lv_opa_t;
using ColorHSV = lv_color_hsv_t;
using EventData = lv_event_t;
using AnimData = lv_anim_t;

// Bad - would hide pointer nature (we don't do this)
// using ChartSeries = lv_chart_series_t*;  // NO!
```

---

## Event System

### Architecture

Events use a CRTP mixin (`EventMixin<Derived>`) that provides:

1. **Free function callbacks** (stateless lambdas)
2. **Member function pointer callbacks** (type-safe, zero allocation)

```cpp
template<typename Derived>
class EventMixin {
public:
    // Stateless lambda (lv_event_t* or lv::Event)
    template<typename F>
    Derived& on_click(F&& fn) noexcept;

    // Member function pointer (zero-cost, recommended for stateful callbacks)
    template<auto MemFn, typename T>
    Derived& on_click(T* instance) noexcept;

    // Generic event registration (lambda)
    template<typename F>
    Derived& on(lv_event_code_t code, F&& fn) noexcept;

    // Generic event registration (member function pointer)
    template<auto MemFn, typename T>
    Derived& on(lv_event_code_t code, T* instance) noexcept;
};
```

### Supported Events

- `on_click()`, `on_pressed()`, `on_released()`
- `on_value_changed()`
- `on_focused()`, `on_defocused()`
- `on_scroll()`, `on_scroll_end()`
- Generic `on()` for any LVGL event code

### Implementation Detail

The system stores a pointer to the instance in LVGL's user data and uses a static trampoline function that casts and calls the member function. No heap allocation occurs.

---

## Reactive State System

### State<T>

A reactive container that triggers UI updates when changed:

```cpp
lv::State<int> counter{0};

// In component build()
lv::Label::create(parent)
    .bind_text(counter, "Count: %d");

// Later, UI auto-updates
counter.set(counter.get() + 1);
```

### Integration with LVGL Observer

`State<T>` wraps `lv_subject_t` from LVGL's observer system:
- Changes notify all bound widgets
- No heap allocation (subject embedded in State object)
- Supports int, bool, color, pointer types

---

## Layout System

### Flexbox (`lv::Flex`)

```cpp
auto row = lv::hbox(parent)
    .gap(10)
    .padding(16)
    .align_items(lv::kFlexAlign::center)
    .justify(lv::kFlexAlign::space_between);

auto col = lv::vbox(parent)
    .gap(8);
    // Use lv::vbox_wrap(parent) for wrapping columns.

// Child flex properties
child.grow(1);
```

### Grid (`lv::Grid`)

```cpp
static int32_t cols[] = {lv::Grid::fr(1), lv::Grid::fr(2), lv::Grid::template_last};
static int32_t rows[] = {50, lv::Grid::content, lv::Grid::template_last};

auto grid = lv::grid(parent)
    .dsc_array(cols, rows)
    .gap(4);

// Place children
lv::grid_cell(child).col(0).row(0).col_span(2);
```

---

## Component Pattern

### CRTP Base Class

```cpp
template<typename Derived>
class Component {
public:
    void mount(ObjectView parent);   // calls Derived::build() via CRTP
    void unmount();

protected:
    lv_obj_t* m_root = nullptr;
    // Derived must implement: ObjectView build(ObjectView parent);
};
```

### Usage

```cpp
class CounterView : public lv::Component<CounterView> {
    lv::State<int> count{0};

public:
    lv::ObjectView build(lv::ObjectView parent) {
        auto root = lv::vbox(parent).padding(16).gap(8);

        lv::Label::create(root).bind_text(count, "Count: %d");

        lv::Button::create(root)
            .text("Increment")
            .on_click<&CounterView::on_increment>(this);

        return root;
    }

private:
    void on_increment(lv::Event) {
        count.set(count.get() + 1);
    }
};
```

---

## Screen Navigation

### Navigator

Stack-based screen management:

```cpp
lv::Navigator nav;

// Set initial screen
nav.set_root(home_screen);

// Push screen with animation
nav.push(settings_screen, lv::screen_anim::move_left, 300);

// Go back to previous screen
nav.back(300);

// Query state
nav.can_back();   // true if depth > 1
nav.current();    // current screen ObjectView
nav.depth();      // stack depth
```

---

## Styling System

### Inline Styles (via StyleMixin)

```cpp
button
    .bg_color(lv::rgb(0x2196F3))
    .text_color(lv::colors::white())
    .radius(8)
    .padding(12);
```

### Style Objects (Reusable)

```cpp
lv::Style button_style;
button_style
    .bg_color(lv::rgb(0x2196F3))
    .radius(8)
    .padding(12);

button.add_style(button_style);
```

---

## Naming Conventions

| Element | Convention | Example |
|---------|------------|---------|
| Classes | PascalCase | `Chart`, `Button`, `Label` |
| Type aliases | PascalCase | `Color`, `Opacity`, `EventData` |
| Global constant namespaces | k-prefixed | `kState`, `kPart`, `kFlag` |
| Constant values | snake_case | `kState::checked`, `kPart::main` |
| Functions | snake_case | `hex()`, `rgb()`, `snprintf()` |
| Namespaces | snake_case | `colors`, `symbol`, `fonts` |
| Member methods | snake_case | `bg_color()`, `fill_width()` |

### Rules

1. **Don't hide pointers** - `lv_chart_series_t*` stays as-is, not aliased
2. **k-prefixed global constants** - `kGradientDirection`, `kBlendMode`, etc.
3. **Trailing underscore for reserved words** - `default_`, `auto_`

---

## File Organization

```
include/lv/
├── lv.hpp                 # Main header (includes everything)
├── core/
│   ├── object.hpp         # ObjectView, Object, constants
│   ├── event.hpp          # EventMixin, Event
│   ├── style.hpp          # StyleMixin, Style
│   ├── color.hpp          # Color utilities
│   ├── font.hpp           # Font handling
│   ├── timer.hpp          # Timer wrapper
│   ├── anim.hpp           # Animation
│   ├── anim_timeline.hpp  # Animation timeline
│   ├── screen.hpp         # Screen, Navigator
│   ├── state.hpp          # Reactive State<T>
│   ├── component.hpp      # Component base
│   ├── fs.hpp             # Filesystem (File, Directory)
│   ├── snapshot.hpp       # Object screenshot capture
│   ├── gridnav.hpp        # Grid keyboard navigation
│   ├── string_utils.hpp   # String utilities
│   └── ...
├── widgets/
│   ├── label.hpp
│   ├── button.hpp
│   ├── chart.hpp
│   └── ... (37 widgets)
├── draw/
│   ├── draw.hpp           # Umbrella header
│   ├── draw_buf.hpp       # DrawBuf RAII wrapper
│   ├── layer.hpp          # Layer wrapper
│   ├── primitives.hpp     # area(), point() helpers
│   ├── draw_rect.hpp      # FillDsc, BorderDsc, RectDsc
│   ├── draw_line.hpp      # LineDsc
│   ├── draw_arc.hpp       # ArcDsc
│   ├── draw_triangle.hpp  # TriangleDsc
│   ├── draw_label.hpp     # LabelDsc, LetterDsc
│   └── draw_image.hpp     # ImageDsc
├── layout/
│   ├── flex.hpp           # hbox, vbox
│   └── grid.hpp           # CSS Grid
└── libs/
    ├── qrcode.hpp
    ├── barcode.hpp
    ├── gif.hpp
    └── gltf.hpp
```

---

## Example: Complete Application

```cpp
#include <lv/lv.hpp>

class MyApp : public lv::Component<MyApp> {
    lv::State<int> counter{0};

public:
    lv::ObjectView build(lv::ObjectView parent) {
        auto root = lv::vbox(parent)
            .fill()
            .padding(20)
            .gap(16)
            .align_items(lv::kFlexAlign::center);

        lv::Label::create(root)
            .text("Hello, LVGL!")
            .text_color(lv::rgb(0x2196F3));  // blue

        lv::Label::create(root)
            .bind_text(counter, "Counter: %d");

        auto buttons = lv::hbox(root).gap(10);

        lv::Button::create(buttons)
            .text(lv::symbol::minus)
            .on_click<&MyApp::decrement>(this);

        lv::Button::create(buttons)
            .text(lv::symbol::plus)
            .on_click<&MyApp::increment>(this);

        return root;
    }

private:
    void increment(lv::Event) { counter.set(counter.get() + 1); }
    void decrement(lv::Event) { counter.set(counter.get() - 1); }
};

int main() {
    lv::init();
    lv::SDLDisplay display(480, 320);

    MyApp app;
    app.mount(lv::screen_active());

    lv::run();
    return 0;
}
```

---

## What's NOT Wrapped (By Design)

| Item | Reason |
|------|--------|
| `lv_chart_series_t*` | Raw pointer - should be visible |
| `LV_SYMBOL_*` macros | Required for string concatenation |

---

## Implementation Details

### CRTP Mixin Pattern

Instead of virtual functions, we use the Curiously Recurring Template Pattern (CRTP) for zero-overhead polymorphism:

```cpp
template<typename Derived>
class EventMixin {
    lv_obj_t* obj() noexcept {
        return static_cast<Derived*>(this)->get();  // Compile-time dispatch
    }
public:
    Derived& on_click(/*...*/) {
        // ...
        return *static_cast<Derived*>(this);  // Return derived type
    }
};

class Button : public ObjectView,          // Getters: get_width(), parent(), scroll_x(), etc.
               public ObjectMixin<Button>,    // Fluent setters: size(), width(), hide(), etc.
               public EventMixin<Button>,     // Event callbacks: on_click(), on(), etc.
               public StyleMixin<Button> {    // Style setters: bg_color(), padding(), etc.
    // Setter methods return Button& for fluent chaining
};
```

### Trampoline Functions

Member function callbacks compile to ~8 instructions, identical to hand-written C:

```cpp
template<auto MemFn, typename T>
struct MemberTrampoline {
    static void callback(lv_event_t* e) {
        auto* instance = static_cast<T*>(lv_event_get_user_data(e));
        (instance->*MemFn)(e);
    }
};
```

The member function pointer `MemFn` is a template parameter, resolved at compile-time with zero runtime storage.

### Component Ownership via Event Descriptors

Components are identified through their delete-event hook rather than `user_data`.
Every component root has `root_delete_cb` registered via `lv_obj_add_event_cb`.
The callback address `&Component<Derived>::root_delete_cb` is unique per `Derived`
type (each template instantiation produces a distinct function). The component
pointer is stored as the event's `user_data`.

To look up the owning component, `owner_from_obj()` scans event descriptors:

```cpp
static Derived* owner_from_obj(lv_obj_t* obj) noexcept {
    if (!obj) return nullptr;
    const uint32_t n = lv_obj_get_event_count(obj);
    for (uint32_t i = 0; i < n; ++i) {
        lv_event_dsc_t* d = lv_obj_get_event_dsc(obj, i);
        if (!d) continue;
        if (lv_event_dsc_get_cb(d) == &Component::root_delete_cb)
            return static_cast<Derived*>(lv_event_dsc_get_user_data(d));
    }
    return nullptr;
}
```

This approach:
- **Eliminates UB**: no type-punning of arbitrary `user_data` pointers.
- **Frees `user_data`**: `lv_obj_t::user_data` on component roots is available for normal use.
- **Shrinks Component**: `sizeof(Component<T>) == sizeof(void*)` (8 bytes on 64-bit).
- Uses only public, stable LVGL APIs (`lv_obj_get_event_count`, `lv_obj_get_event_dsc`,
  `lv_event_dsc_get_cb`, `lv_event_dsc_get_user_data`).

### Component Root user_data

Since components no longer occupy `user_data`, the root's `user_data` is freely
available for application use.

| Context | Set | Get | Typed get |
|---------|-----|-----|-----------|
| Widget (Button, Grid, etc.) | `.user_data(ptr)` | `.user_data()` | `.user_data_as<T>()` |
| Raw `ObjectView` / `root()` | `lv_obj_set_user_data(obj, ptr)` | `.get_user_data()` | `.get_user_data<T>()` |

Widgets inherit setter and getter overloads from `ObjectMixin`. Raw `ObjectView` (e.g. from
`parent()`, `child()`, `Component::root()`) uses `get_user_data()` — a different name
to avoid ambiguity with `ObjectMixin::user_data()` in multiple inheritance.

> **Note:** Most read-only getters (`get_width()`, `get_height()`, `scroll_x()`, etc.)
> live on `ObjectView`, not `ObjectMixin`. The `user_data` getter is an exception because
> of the naming conflict described above. See `docs/DESIGN_OBJECTVIEW_GETTERS.md`.

---

## Future Considerations

1. **Reconciler/diff engine** - For efficient virtual tree updates
2. **Key-based identity** - For list item stability
3. **Frame scheduling** - Coalesce state changes per frame

---

## References

- LVGL Documentation: https://docs.lvgl.io/
- [Naming Conventions](NAMING_CONVENTIONS.md)
- [API Guide](CPP_API_GUIDE.md)

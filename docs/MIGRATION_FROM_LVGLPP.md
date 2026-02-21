# Migrating From `lvglpp` to `lv`

This guide is for projects moving from the old `lvglpp` wrapper (in `../lvglpp`) to this `lv` C++ wrapper.

## Scope

- `lvglpp` was written around LVGL 8-era APIs and ownership patterns.
- `lv` is designed around LVGL 9 APIs, zero-cost views, and fluent chaining.
- This is not a drop-in replacement. Expect mechanical API edits.

## Core Mental Model Changes

1. Ownership model
- `lvglpp`: wrappers are often owning (`owns_ptr`, `release_ptr` patterns).
- `lv`: widget wrappers are usually lightweight views over LVGL-managed objects.
- Parent-child ownership in LVGL is the default lifecycle.
- `lv::Object` exists, but most app code should use widget wrappers + `ObjectView`.

2. Object creation
- `lvglpp`: constructor style, e.g. `Button(parent)`.
- `lv`: explicit factory style, e.g. `lv::Button::create(parent)`.

3. API style
- `lvglpp`: mostly `set_*`, `get_*`.
- `lv`: fluent short methods like `.size(...)`, `.text(...)`, `.align(...)`.

4. Events
- `lvglpp`: `add_event_cb(cb, LV_EVENT_...)`.
- `lv`: `on(...)`, `on_click(...)`, member-function trampolines.

5. Includes and feature flags
- `lvglpp`: many per-module includes.
- `lv`: usually `#include <lv/lv.hpp>`, with `LV_USE_*` guards respected.

## Quick Mapping

| `lvglpp` | `lv` |
|---|---|
| `#include "lvglpp/lvglpp.h"` | `#include <lv/lv.hpp>` |
| `lvgl::init()` | `lv::init()` |
| `scr_act()` / `screen_active()` | `lv::screen_active()` |
| `Button(parent)` | `lv::Button::create(parent)` |
| `Label(parent)` | `lv::Label::create(parent)` |
| `set_size(w,h)` | `size(w,h)` |
| `set_pos(x,y)` | `pos(x,y)` |
| `set_text("...")` | `text("...")` |
| `set_text_fmt(...)` | `text_fmt(...)` |
| `clear_flag(f)` | `remove_flag(f)` |
| `remove_style_all()` | `remove_all_styles()` |
| `raw_ptr()` | `get()` |
| `add_event_cb(cb, LV_EVENT_CLICKED)` | `on_click(...)` or `on(lv::kEvent::clicked, ...)` |
| `MessageBox` | `lv::Msgbox` |
| `TextArea` | `lv::Textarea` |
| `SpanGroup` | `lv::Spangroup` |
| `AnimatedImage` | `lv::AnimImage` |

## Before/After Examples

### 1) Button + click event

```cpp
// lvglpp
static auto btn = Button(scr_act());
btn.set_size(100, 50);
btn.center();
btn.add_event_cb([](Event& e) {
    auto b = e.get_target<Button>();
    auto l = b.get_child<Label>(0);
    l.set_text("Clicked");
}, LV_EVENT_CLICKED);
```

```cpp
// lv
auto btn = lv::Button::create(lv::screen_active())
    .size(100, 50)
    .center()
    .on_click([](lv::Event e) {
        auto b = lv::Button(lv::wrap, e.target().get());
        b.text("Clicked");
    });
```

### 2) Text label

```cpp
// lvglpp
auto label = Label(btn);
label.set_text("Hello");
label.center();
```

```cpp
// lv
lv::Label::create(btn)
    .text("Hello")
    .center();
```

### 3) Timer

```cpp
// lvglpp: usually subclass Timer and override callback()
```

```cpp
// lv: type-safe member callback
m_timer = lv::Timer::create<&MyScreen::on_tick>(1000, this);
```

## Event Migration Notes

- `lv::Event` is explicit and typed (`e.target()`, `e.code()`, `e.user_data()`).
- For stateful handlers, prefer member function callbacks:
  - `obj.on<&MyType::handler>(lv::kEvent::clicked, this);`
- Stateless lambdas are supported directly.

## Lifecycle Migration Notes

- Remove most `release_ptr()`-style code. Usually unnecessary in `lv`.
- Avoid storing wrappers solely to prevent accidental deletion (common in `lvglpp`).
- Keep references only when you need to update widgets later.

## Known Naming Differences

- `MessageBox` -> `Msgbox`
- `TextArea` -> `Textarea`
- `SpanGroup` -> `Spangroup`
- `AnimatedImage` -> `AnimImage`

## Missing or Different Coverage

Some `lvglpp` wrappers may not exist yet in `lv` under the same name or shape (for example old `Meter`/`ColorWheel` usage patterns). For those:

1. Use raw LVGL C API temporarily via `.get()`.
2. Add a local wrapper helper.
3. Upstream the wrapper if it is generally useful.

## Recommended Migration Plan

1. First compile your project on LVGL 9 C API names.
2. Switch includes to `lv` and replace constructors with `::create(...)`.
3. Convert high-frequency calls (`set_*` -> fluent API).
4. Migrate events to `on...` APIs.
5. Remove old ownership workarounds (`release_ptr`, static wrappers used only for lifetime).
6. Re-enable only needed `LV_USE_*` flags and compile.

## Common Pitfalls

- Assuming every old class name exists unchanged.
- Expecting constructor-based widget creation.
- Forgetting `LV_USE_*` dependency chains in LVGL itself (some widgets require others).
- Mixing old LVGL v8 enum/function names with LVGL v9 APIs.

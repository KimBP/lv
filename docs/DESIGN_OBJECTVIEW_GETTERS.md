# Design: ObjectView getters vs ObjectMixin setters

## The problem

`parent()` and `child()` return `ObjectView`. But geometry methods like
`get_width()` and `get_height()` lived in `ObjectMixin<Derived>`, which
`ObjectView` does not inherit. So this didn't work:

```cpp
auto p = btn.parent();
int32_t w = p.get_width();  // Error: ObjectView has no member get_width
```

Users had to drop to the C API:

```cpp
int32_t w = lv_obj_get_width(btn.parent());
```

## The fix

Move all read-only getters from `ObjectMixin` into `ObjectView`. Keep
all fluent setters (returning `Derived&`) in `ObjectMixin`.

**ObjectView** (read — works on any object handle):

| Method | Return |
|--------|--------|
| `get_width()`, `get_height()` | `int32_t` |
| `content_width()`, `content_height()` | `int32_t` |
| `get_coords(lv_area_t*)` | `void` |
| `scroll_x()`, `scroll_y()` | `int32_t` |
| `calculate_ext_draw_size()` | `int32_t` |
| `has_state()`, `has_flag()` | `bool` |
| `parent()`, `child()`, `child_count()` | `ObjectView` / `uint32_t` |
| `get_user_data()`, `get_user_data<T>()` | `void*` / `T*` |

**ObjectMixin\<Derived\>** (write — fluent chaining):

| Method | Return |
|--------|--------|
| `width()`, `height()`, `size()` | `Derived&` |
| `pos()`, `x()`, `y()`, `align()`, `center()` | `Derived&` |
| `hide()`, `show()`, `visible()` | `Derived&` |
| `add_flag()`, `remove_flag()` | `Derived&` |
| `user_data(void*)` | `Derived&` |
| `user_data()`, `user_data_as<T>()` | `void*` / `T*` |
| `scroll_by()`, `scroll_to()` | `Derived&` |
| `name()`, `get_name()` | `Derived&` / `const char*` |
| ... all other setters | `Derived&` |

Now this works:

```cpp
int32_t w = btn.parent().get_width();
int32_t h = btn.child(0).content_height();
auto sy = display.layer_top().scroll_y();
```

## Why `user_data` has two names

ObjectView has `get_user_data()`. ObjectMixin has `user_data()` (getter)
and `user_data(void*)` (setter). They return the same value but use
different names. This is forced by C++ multiple inheritance rules.

Widgets inherit from both ObjectView and ObjectMixin as **unrelated
bases** (sibling inheritance):

```
ObjectView    ObjectMixin<Button>
     \              /
      \            /
        Button
```

If both bases declare the same name `user_data`, the compiler finds it
in two unrelated scopes and reports ambiguity — even if the overloads
have different signatures. Name hiding only works within a single
inheritance chain (parent-child), not across siblings.

So the different names (`get_user_data` vs `user_data`) are required:

- On a bare `ObjectView` (from `parent()`, `child()`): use `get_user_data()`
- On a widget: use `user_data()` / `user_data_as<T>()`

## Known limitations

### 1. `get_name()` stays in ObjectMixin

`lv_obj_get_name` only exists when `LV_USE_OBJ_NAME` is enabled. In
`ObjectMixin` (a template), `if constexpr` discards the branch and the
missing symbol is never instantiated. In `ObjectView` (a concrete class),
the compiler must resolve all symbols even in dead `if constexpr` branches.

So `obj.parent().get_name()` does not compile. Use a typed widget, or
call the C API directly.

### 2. No null assert on ObjectView

`ObjectMixin` methods go through a private `obj()` helper that calls
`LV_ASSERT_NULL`. `ObjectView` methods use `m_obj` directly. Calling
`ObjectView{nullptr}.get_width()` passes nullptr to LVGL with no assert.

This is consistent with existing ObjectView methods (`parent()`,
`has_state()`, etc.) which also have no null guard — the pattern is that
ObjectView trusts the caller to hold a valid pointer.

### 3. ObjectView API surface grew

ObjectView went from ~10 methods to ~18. Still zero-cost at runtime
(no storage change, same `sizeof(void*)`), but no longer a minimal
pointer wrapper. This is a deliberate trade-off: ObjectView is now a
useful read-only handle rather than a nearly-empty shell.

## Design rule

**Getters go on ObjectView. Setters go on ObjectMixin.**

When adding new LVGL wrappers, follow this split:

- If it returns a value and takes no "set" argument → `ObjectView`
- If it returns `Derived&` for chaining → `ObjectMixin`
- If the underlying C function doesn't exist in all configs → `ObjectMixin`
  (where `if constexpr` can discard the branch)
- If ObjectMixin has a setter with the same name → use a `get_` prefix
  on ObjectView to avoid multiple-inheritance ambiguity

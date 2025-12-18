#!/bin/bash
# Check API coverage: compare LVGL C functions used in examples vs our C++ wrappers

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# LVGL examples path - check sibling directory or use LVGL_DIR env var
LVGL_DIR="${LVGL_DIR:-$(dirname "$PROJECT_ROOT")/lvgl}"
LVGL_EXAMPLES="$LVGL_DIR/examples"
LV_INCLUDE="$PROJECT_ROOT/include/lv"

if [ ! -d "$LVGL_EXAMPLES" ]; then
    echo "Error: LVGL examples not found at $LVGL_EXAMPLES"
    echo "Set LVGL_DIR environment variable or place lvgl as sibling directory"
    exit 1
fi

echo "=== LVGL C++ Wrapper API Coverage Check ==="
echo ""

# Extract all lv_* function calls from LVGL examples
echo "Extracting functions from LVGL examples..."
EXAMPLE_FUNCS=$(grep -roh 'lv_[a-z_]*(' "$LVGL_EXAMPLES/widgets/" 2>/dev/null | \
    sed 's/($//' | sort -u)

TOTAL_FUNCS=$(echo "$EXAMPLE_FUNCS" | wc -l)
echo "Found $TOTAL_FUNCS unique lv_* functions in examples"
echo ""

# Group by widget type
echo "=== Functions by Widget ==="
for widget in obj label button arc bar slider switch checkbox dropdown \
              roller textarea spinbox chart table list menu msgbox \
              keyboard calendar canvas image line led scale span; do
    count=$(echo "$EXAMPLE_FUNCS" | grep -c "^lv_${widget}_" || true)
    if [ "$count" -gt 0 ]; then
        echo "$widget: $count functions"
    fi
done

echo ""
echo "=== Style functions ==="
style_count=$(echo "$EXAMPLE_FUNCS" | grep -c "^lv_style_" || true)
echo "lv_style_*: $style_count functions"

obj_style_count=$(echo "$EXAMPLE_FUNCS" | grep -c "^lv_obj_set_style_" || true)
echo "lv_obj_set_style_*: $obj_style_count functions"

echo ""
echo "=== Animation functions ==="
anim_count=$(echo "$EXAMPLE_FUNCS" | grep -c "^lv_anim_" || true)
echo "lv_anim_*: $anim_count functions"

echo ""
echo "=== Checking wrapper coverage for key widgets ==="

check_widget() {
    local widget=$1
    local wrapper_file=$2

    if [ ! -f "$wrapper_file" ]; then
        echo "  $widget: wrapper file not found ($wrapper_file)"
        return
    fi

    # Get functions used in examples for this widget
    local example_funcs=$(echo "$EXAMPLE_FUNCS" | grep "^lv_${widget}_")
    local example_count=$(echo "$example_funcs" | grep -c . || echo 0)

    # Check how many are wrapped (look for the function name without lv_ prefix)
    local wrapped=0
    local missing=""

    while IFS= read -r func; do
        [ -z "$func" ] && continue
        # Convert lv_widget_action to just action
        local method=$(echo "$func" | sed "s/^lv_${widget}_//")

        # Check if method exists in wrapper (as function name)
        if grep -q "$method\|$func" "$wrapper_file" 2>/dev/null; then
            ((wrapped++))
        else
            missing="$missing $func"
        fi
    done <<< "$example_funcs"

    echo "  $widget: $wrapped/$example_count wrapped"
    if [ -n "$missing" ]; then
        echo "    Missing:$missing" | fold -s -w 70 | sed 's/^/      /'
    fi
}

check_widget "arc" "$LV_INCLUDE/widgets/arc.hpp"
check_widget "bar" "$LV_INCLUDE/widgets/bar.hpp"
check_widget "button" "$LV_INCLUDE/widgets/button.hpp"
check_widget "label" "$LV_INCLUDE/widgets/label.hpp"
check_widget "slider" "$LV_INCLUDE/widgets/slider.hpp"
check_widget "switch" "$LV_INCLUDE/widgets/switch.hpp"
check_widget "chart" "$LV_INCLUDE/widgets/chart.hpp"
check_widget "dropdown" "$LV_INCLUDE/widgets/dropdown.hpp"
check_widget "checkbox" "$LV_INCLUDE/widgets/checkbox.hpp"

echo ""
echo "=== Full function list (for reference) ==="
echo "$EXAMPLE_FUNCS" | head -100
echo "... ($(echo "$EXAMPLE_FUNCS" | wc -l) total)"

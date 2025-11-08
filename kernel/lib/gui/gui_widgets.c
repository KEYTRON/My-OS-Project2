/*
 * GUI Widget System Implementation
 */

#include "gui_widgets.h"
#include "../graphics/graphics_primitives.h"
#include "../graphics/graphics.h"
#include "../graphics/graphics_font.h"
#include "../string.h"

/* str_ncpy implementation since we're in freestanding mode */
static void str_ncpy(char *dst, const char *src, uint32_t n) {
    while (n > 0 && *src) {
        *dst++ = *src++;
        n--;
    }
    if (n > 0) *dst = '\0';
}

/* Simple memory allocation stub - for now use stack or globals */
void *kmalloc(uint32_t size) {
    /* TODO: Implement proper kernel malloc */
    static uint8_t heap[65536];
    static uint32_t heap_pos = 0;

    if (heap_pos + size > sizeof(heap)) {
        return NULL;
    }

    void *ptr = &heap[heap_pos];
    heap_pos += size;
    return ptr;
}

void kfree(void *ptr) {
    /* TODO: Implement proper kernel free */
    (void)ptr;
}

/* Global GUI state */
static struct {
    gui_widget_t *windows[MAX_WINDOWS];
    uint32_t window_count;
    gui_widget_t *focused_widget;
    gui_widget_t *hovered_widget;

    gui_event_t event_queue[256];
    uint32_t event_head;
    uint32_t event_tail;

    uint32_t next_widget_id;
} gui_state = {0};

/* ============================================
 * Widget Initialization
 * ============================================ */

void gui_init(void) {
    memset(&gui_state, 0, sizeof(gui_state));
    gui_state.next_widget_id = 1000;
}

/* ============================================
 * Widget Creation and Management
 * ============================================ */

gui_widget_t *gui_widget_create(widget_type_t type, const char *id) {
    gui_widget_t *w = (gui_widget_t *)kmalloc(sizeof(gui_widget_t));
    if (!w) return NULL;

    memset(w, 0, sizeof(gui_widget_t));

    w->id = gui_state.next_widget_id++;
    w->type = type;
    w->flags = WIDGET_VISIBLE | WIDGET_ENABLED;
    w->background_color = DEFAULT_BG_COLOR;
    w->foreground_color = DEFAULT_FG_COLOR;
    w->border_color = COLOR_GRAY;
    w->border_width = 1;

    return w;
}

void gui_widget_destroy(gui_widget_t *w) {
    if (!w) return;

    /* Destroy children */
    for (uint32_t i = 0; i < w->child_count; i++) {
        gui_widget_destroy(w->children[i]);
    }

    /* Call type-specific cleanup */
    if (w->vtable && w->vtable->on_destroy) {
        w->vtable->on_destroy(w);
    }

    /* Free type-specific data */
    if (w->data) {
        kfree(w->data);
    }

    kfree(w);
}

void gui_widget_add_child(gui_widget_t *parent, gui_widget_t *child) {
    if (!parent || !child) return;
    if (parent->child_count >= MAX_WIDGETS_PER_WINDOW) return;

    parent->children[parent->child_count++] = child;
    child->parent = parent;
    gui_widget_invalidate(parent);
}

void gui_widget_remove_child(gui_widget_t *parent, gui_widget_t *child) {
    if (!parent || !child) return;

    for (uint32_t i = 0; i < parent->child_count; i++) {
        if (parent->children[i] == child) {
            /* Shift remaining children */
            for (uint32_t j = i; j < parent->child_count - 1; j++) {
                parent->children[j] = parent->children[j + 1];
            }
            parent->child_count--;
            gui_widget_invalidate(parent);
            return;
        }
    }
}

void gui_widget_set_bounds(gui_widget_t *w, int32_t x, int32_t y,
                           uint32_t width, uint32_t height) {
    if (!w) return;

    w->bounds.x = x;
    w->bounds.y = y;
    w->bounds.width = width;
    w->bounds.height = height;
    gui_widget_invalidate(w);
}

void gui_widget_set_visible(gui_widget_t *w, bool visible) {
    if (!w) return;

    if (visible) {
        w->flags |= WIDGET_VISIBLE;
    } else {
        w->flags &= ~WIDGET_VISIBLE;
    }
    gui_widget_invalidate(w);
}

void gui_widget_set_enabled(gui_widget_t *w, bool enabled) {
    if (!w) return;

    if (enabled) {
        w->flags |= WIDGET_ENABLED;
    } else {
        w->flags &= ~WIDGET_ENABLED;
    }
}

void gui_widget_invalidate(gui_widget_t *w) {
    if (!w) return;
    w->flags |= WIDGET_DIRTY;

    if (w->parent) {
        gui_widget_invalidate(w->parent);
    }
}

void gui_widget_paint(gui_widget_t *w) {
    if (!w || !(w->flags & WIDGET_VISIBLE)) return;

    /* Call type-specific paint */
    if (w->vtable && w->vtable->paint) {
        w->vtable->paint(w);
    }

    /* Paint children */
    for (uint32_t i = 0; i < w->child_count; i++) {
        gui_widget_paint(w->children[i]);
    }

    w->flags &= ~WIDGET_DIRTY;
}

void gui_widget_handle_event(gui_widget_t *w, gui_event_t *e) {
    if (!w || !(w->flags & WIDGET_VISIBLE) || !(w->flags & WIDGET_ENABLED)) return;

    /* Call type-specific event handler */
    if (w->vtable && w->vtable->on_event) {
        w->vtable->on_event(w, e);
    }
}

gui_widget_t *gui_widget_hit_test(gui_widget_t *w, int32_t x, int32_t y) {
    if (!w || !(w->flags & WIDGET_VISIBLE)) return NULL;

    /* Check if point is within bounds */
    if (x < w->bounds.x || x >= w->bounds.x + (int32_t)w->bounds.width ||
        y < w->bounds.y || y >= w->bounds.y + (int32_t)w->bounds.height) {
        return NULL;
    }

    /* Check children in reverse order (top-most first) */
    for (int32_t i = w->child_count - 1; i >= 0; i--) {
        gui_widget_t *hit = gui_widget_hit_test(w->children[i], x, y);
        if (hit) return hit;
    }

    return w;
}

/* ============================================
 * Window Management
 * ============================================ */

static void window_paint(gui_widget_t *w) {
    window_data_t *data = (window_data_t *)w->data;
    if (!data) return;

    /* Draw window background */
    graphics_rect_t bg = {w->bounds.x, w->bounds.y, w->bounds.width, w->bounds.height};
    graphics_fillrect(bg, w->background_color);

    /* Draw title bar */
    graphics_rect_t titlebar = {w->bounds.x, w->bounds.y, w->bounds.width, data->title_height};
    graphics_fillrect(titlebar, 0xFF404080);  /* Blue-gray */

    /* Draw title text */
    if (w->flags & WIDGET_FOCUSED) {
        graphics_draw_string_at(w->bounds.x + 8, w->bounds.y + 5, data->title, COLOR_WHITE, 0xFF404080);
    } else {
        graphics_draw_string_at(w->bounds.x + 8, w->bounds.y + 5, data->title, 0xFFAAAAAA, 0xFF404080);
    }

    /* Draw window border */
    if (w->border_width > 0) {
        graphics_point_t p1 = {w->bounds.x, w->bounds.y};
        graphics_point_t p2 = {w->bounds.x + (int32_t)w->bounds.width, w->bounds.y};
        graphics_drawline(p1, p2, w->border_color);
    }

    /* Draw close button if enabled */
    if (data->has_close_btn) {
        int32_t btn_x = w->bounds.x + w->bounds.width - 20;
        int32_t btn_y = w->bounds.y + 4;
        graphics_rect_t btn_rect = {btn_x, btn_y, 16, 16};
        graphics_fillrect(btn_rect, 0xFFCC0000);  /* Red */
        graphics_draw_string_at(btn_x + 3, btn_y + 2, "X", COLOR_WHITE, 0xFFCC0000);
    }
}

static void window_on_event(gui_widget_t *w, gui_event_t *e) {
    window_data_t *data = (window_data_t *)w->data;
    if (!data) return;

    switch (e->type) {
        case EVENT_MOUSE_CLICK:
            if (e->data.mouse.y < w->bounds.y + (int32_t)data->title_height) {
                /* Click in title bar - start drag */
                data->is_dragging = true;
                data->drag_start_x = e->data.mouse.x;
                data->drag_start_y = e->data.mouse.y;

                /* Check for close button */
                if (data->has_close_btn) {
                    int32_t btn_x = w->bounds.x + w->bounds.width - 20;
                    if (e->data.mouse.x >= btn_x && e->data.mouse.x < btn_x + 16) {
                        gui_window_close(w);
                    }
                }
            }
            break;

        case EVENT_MOUSE_RELEASE:
            data->is_dragging = false;
            break;

        case EVENT_MOUSE_MOVE:
            if (data->is_dragging) {
                int32_t dx = e->data.mouse.x - data->drag_start_x;
                int32_t dy = e->data.mouse.y - data->drag_start_y;

                w->bounds.x += dx;
                w->bounds.y += dy;

                data->drag_start_x = e->data.mouse.x;
                data->drag_start_y = e->data.mouse.y;

                gui_widget_invalidate(w);
            }
            break;

        case EVENT_FOCUS:
            w->flags |= WIDGET_FOCUSED;
            gui_window_bring_to_front(w);
            gui_widget_invalidate(w);
            break;

        case EVENT_BLUR:
            w->flags &= ~WIDGET_FOCUSED;
            gui_widget_invalidate(w);
            break;

        default:
            break;
    }
}

gui_widget_t *gui_window_create(const char *title,
                                int32_t x, int32_t y,
                                uint32_t width, uint32_t height) {
    gui_widget_t *w = gui_widget_create(WIDGET_WINDOW, title);
    if (!w) return NULL;

    window_data_t *data = (window_data_t *)kmalloc(sizeof(window_data_t));
    if (!data) {
        gui_widget_destroy(w);
        return NULL;
    }

    memset(data, 0, sizeof(window_data_t));
    str_ncpy(data->title, title, sizeof(data->title) - 1);
    data->title_height = 24;
    data->has_close_btn = true;
    data->has_minimize_btn = false;

    w->data = data;
    w->vtable = &(widget_vtable_t){
        .paint = window_paint,
        .on_event = window_on_event,
        .on_destroy = NULL
    };

    gui_widget_set_bounds(w, x, y, width, height);

    /* Add to window list */
    if (gui_state.window_count < MAX_WINDOWS) {
        gui_state.windows[gui_state.window_count++] = w;
    }

    return w;
}

void gui_window_set_title(gui_widget_t *window, const char *title) {
    if (!window || window->type != WIDGET_WINDOW) return;

    window_data_t *data = (window_data_t *)window->data;
    if (!data) return;

    str_ncpy(data->title, title, sizeof(data->title) - 1);
    gui_widget_invalidate(window);
}

void gui_window_bring_to_front(gui_widget_t *window) {
    if (!window || window->type != WIDGET_WINDOW) return;

    /* Find and remove window from list */
    for (uint32_t i = 0; i < gui_state.window_count; i++) {
        if (gui_state.windows[i] == window) {
            /* Move to end (front) */
            for (uint32_t j = i; j < gui_state.window_count - 1; j++) {
                gui_state.windows[j] = gui_state.windows[j + 1];
            }
            gui_state.windows[gui_state.window_count - 1] = window;
            gui_widget_invalidate(window);
            return;
        }
    }
}

void gui_window_minimize(gui_widget_t *window) {
    if (!window || window->type != WIDGET_WINDOW) return;
    gui_widget_set_visible(window, false);
}

void gui_window_maximize(gui_widget_t *window) {
    if (!window || window->type != WIDGET_WINDOW) return;

    /* TODO: Maximize to screen size */
    uint32_t screen_width = 1280;
    uint32_t screen_height = 1024;
    gui_widget_set_bounds(window, 0, 0, screen_width, screen_height);
}

void gui_window_close(gui_widget_t *window) {
    if (!window || window->type != WIDGET_WINDOW) return;

    /* Remove from window list */
    for (uint32_t i = 0; i < gui_state.window_count; i++) {
        if (gui_state.windows[i] == window) {
            for (uint32_t j = i; j < gui_state.window_count - 1; j++) {
                gui_state.windows[j] = gui_state.windows[j + 1];
            }
            gui_state.window_count--;
            break;
        }
    }

    gui_widget_destroy(window);
}

/* ============================================
 * Button Management
 * ============================================ */

static void button_paint(gui_widget_t *w) {
    button_data_t *data = (button_data_t *)w->data;
    if (!data) return;

    /* Draw button background */
    uint32_t bg_color = DEFAULT_BTN_COLOR;
    if (w->flags & WIDGET_HOVERED) {
        bg_color = DEFAULT_BTN_HOVER;
    }
    if (w->flags & WIDGET_FOCUSED) {
        bg_color = DEFAULT_BTN_ACTIVE;
    }

    graphics_rect_t btn_rect = {w->bounds.x, w->bounds.y, w->bounds.width, w->bounds.height};
    graphics_fillrect(btn_rect, bg_color);

    /* Draw button border */
    graphics_point_t tl = {w->bounds.x, w->bounds.y};
    graphics_point_t tr = {w->bounds.x + (int32_t)w->bounds.width, w->bounds.y};
    graphics_drawline(tl, tr, COLOR_GRAY);

    /* Draw button text */
    int32_t text_x = w->bounds.x + (w->bounds.width / 2) - 20;
    int32_t text_y = w->bounds.y + (w->bounds.height / 2) - 4;
    graphics_draw_string_at(text_x, text_y, data->text, data->text_color, bg_color);
}

static void button_on_event(gui_widget_t *w, gui_event_t *e) {
    switch (e->type) {
        case EVENT_MOUSE_CLICK:
            if (w->on_click) {
                w->on_click(w, e->data.mouse.x, e->data.mouse.y);
            }
            gui_widget_invalidate(w);
            break;

        case EVENT_MOUSE_MOVE: {
            bool was_hovered = (w->flags & WIDGET_HOVERED) != 0;
            bool is_hovered = gui_widget_hit_test(w, e->data.mouse.x, e->data.mouse.y) == w;

            if (is_hovered && !was_hovered) {
                w->flags |= WIDGET_HOVERED;
                gui_widget_invalidate(w);
            } else if (!is_hovered && was_hovered) {
                w->flags &= ~WIDGET_HOVERED;
                gui_widget_invalidate(w);
            }
            break;
        }

        default:
            break;
    }
}

gui_widget_t *gui_button_create(const char *text) {
    gui_widget_t *w = gui_widget_create(WIDGET_BUTTON, text);
    if (!w) return NULL;

    button_data_t *data = (button_data_t *)kmalloc(sizeof(button_data_t));
    if (!data) {
        gui_widget_destroy(w);
        return NULL;
    }

    memset(data, 0, sizeof(button_data_t));
    str_ncpy(data->text, text, sizeof(data->text) - 1);
    data->text_color = COLOR_WHITE;

    w->data = data;
    w->vtable = &(widget_vtable_t){
        .paint = button_paint,
        .on_event = button_on_event,
        .on_destroy = NULL
    };

    gui_widget_set_bounds(w, 0, 0, 100, 30);

    return w;
}

void gui_button_set_text(gui_widget_t *button, const char *text) {
    if (!button || button->type != WIDGET_BUTTON) return;

    button_data_t *data = (button_data_t *)button->data;
    if (!data) return;

    str_ncpy(data->text, text, sizeof(data->text) - 1);
    gui_widget_invalidate(button);
}

void gui_button_set_onclick(gui_widget_t *button,
                            void (*callback)(gui_widget_t *)) {
    if (!button) return;
    button->on_click = (void (*)(gui_widget_t *, int32_t, int32_t))callback;
}

/* ============================================
 * Label Management
 * ============================================ */

static void label_paint(gui_widget_t *w) {
    label_data_t *data = (label_data_t *)w->data;
    if (!data) return;

    graphics_draw_string_at(w->bounds.x, w->bounds.y, data->text, data->text_color, COLOR_BLACK);
}

gui_widget_t *gui_label_create(const char *text) {
    gui_widget_t *w = gui_widget_create(WIDGET_LABEL, text);
    if (!w) return NULL;

    label_data_t *data = (label_data_t *)kmalloc(sizeof(label_data_t));
    if (!data) {
        gui_widget_destroy(w);
        return NULL;
    }

    memset(data, 0, sizeof(label_data_t));
    str_ncpy(data->text, text, sizeof(data->text) - 1);
    data->text_color = COLOR_WHITE;

    w->data = data;
    w->vtable = &(widget_vtable_t){
        .paint = label_paint,
        .on_event = NULL,
        .on_destroy = NULL
    };

    gui_widget_set_bounds(w, 0, 0, 200, 16);

    return w;
}

void gui_label_set_text(gui_widget_t *label, const char *text) {
    if (!label || label->type != WIDGET_LABEL) return;

    label_data_t *data = (label_data_t *)label->data;
    if (!data) return;

    str_ncpy(data->text, text, sizeof(data->text) - 1);
    gui_widget_invalidate(label);
}

/* ============================================
 * Event Management
 * ============================================ */

void gui_process_events(void) {
    while (gui_state.event_head != gui_state.event_tail) {
        gui_event_t *event = &gui_state.event_queue[gui_state.event_head];
        gui_state.event_head = (gui_state.event_head + 1) % 256;

        if (event->target) {
            gui_widget_handle_event(event->target, event);
        }
    }
}

void gui_render(void) {
    /* Draw all windows */
    for (uint32_t i = 0; i < gui_state.window_count; i++) {
        gui_widget_paint(gui_state.windows[i]);
    }
}

void gui_post_event(gui_event_t *event) {
    uint32_t next_tail = (gui_state.event_tail + 1) % 256;
    if (next_tail == gui_state.event_head) {
        return;  /* Queue full */
    }

    gui_state.event_queue[gui_state.event_tail] = *event;
    gui_state.event_tail = next_tail;
}

void gui_inject_mouse_event(int32_t x, int32_t y,
                           mouse_button_t button, bool pressed) {
    gui_event_t event = {0};

    event.type = pressed ? EVENT_MOUSE_CLICK : EVENT_MOUSE_RELEASE;
    event.data.mouse.x = x;
    event.data.mouse.y = y;
    event.data.mouse.button = button;

    /* Find window under cursor */
    event.target = NULL;
    for (uint32_t i = gui_state.window_count - 1; i > 0; i--) {
        gui_widget_t *hit = gui_widget_hit_test(gui_state.windows[i], x, y);
        if (hit) {
            event.target = hit;
            break;
        }
    }

    if (event.target) {
        gui_post_event(&event);
    }
}

void gui_inject_key_event(key_code_t code, char ascii, bool pressed) {
    gui_event_t event = {0};

    event.type = pressed ? EVENT_KEY_PRESS : EVENT_KEY_RELEASE;
    event.target = gui_state.focused_widget;
    event.data.key.code = code;
    event.data.key.ascii = ascii;

    if (event.target) {
        gui_post_event(&event);
    }
}

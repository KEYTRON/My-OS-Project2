/*
 * GUI Widget System for MyOS
 * Provides basic UI components: windows, buttons, text fields, etc.
 */

#ifndef GUI_WIDGETS_H
#define GUI_WIDGETS_H

#include <stdint.h>
#include <stdbool.h>
#include "../graphics/graphics.h"

/* Maximum widgets per window */
#define MAX_WIDGETS_PER_WINDOW 32
#define MAX_WINDOWS 16

/* Widget types */
typedef enum {
    WIDGET_WINDOW,
    WIDGET_BUTTON,
    WIDGET_LABEL,
    WIDGET_TEXTBOX,
    WIDGET_PANEL,
    WIDGET_LISTBOX,
    WIDGET_CHECKBOX,
    WIDGET_RADIO
} widget_type_t;

/* Widget flags */
#define WIDGET_VISIBLE     0x01
#define WIDGET_ENABLED     0x02
#define WIDGET_FOCUSED     0x04
#define WIDGET_HOVERED     0x08
#define WIDGET_DIRTY       0x10  /* Needs redraw */

/* Mouse button states */
typedef enum {
    MOUSE_BTN_LEFT = 1,
    MOUSE_BTN_MIDDLE = 2,
    MOUSE_BTN_RIGHT = 4
} mouse_button_t;

/* Keyboard key codes */
typedef enum {
    KEY_ENTER = 13,
    KEY_ESC = 27,
    KEY_BACKSPACE = 8,
    KEY_TAB = 9,
    KEY_DELETE = 127,
    KEY_UP = 200,
    KEY_DOWN = 201,
    KEY_LEFT = 202,
    KEY_RIGHT = 203
} key_code_t;

/* Rectangle structure */
typedef struct {
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
} rect_t;

/* Event types */
typedef enum {
    EVENT_MOUSE_CLICK,
    EVENT_MOUSE_RELEASE,
    EVENT_MOUSE_MOVE,
    EVENT_KEY_PRESS,
    EVENT_KEY_RELEASE,
    EVENT_FOCUS,
    EVENT_BLUR,
    EVENT_PAINT
} event_type_t;

/* Event structure */
typedef struct gui_widget gui_widget_t;

typedef struct {
    event_type_t type;
    gui_widget_t *target;

    union {
        struct {
            int32_t x;
            int32_t y;
            mouse_button_t button;
        } mouse;

        struct {
            key_code_t code;
            char ascii;
        } key;

        struct {
            uint32_t width;
            uint32_t height;
        } paint;
    } data;
} gui_event_t;

/* Widget vtable for polymorphism */
typedef struct {
    void (*paint)(gui_widget_t *w);
    void (*on_event)(gui_widget_t *w, gui_event_t *e);
    void (*on_destroy)(gui_widget_t *w);
    void (*measure)(gui_widget_t *w, uint32_t *width, uint32_t *height);
} widget_vtable_t;

/* Base widget structure */
typedef struct gui_widget {
    uint32_t id;
    widget_type_t type;
    uint8_t flags;

    rect_t bounds;
    uint32_t background_color;
    uint32_t foreground_color;
    uint32_t border_color;
    uint8_t border_width;

    void *data;                    /* Type-specific data */
    widget_vtable_t *vtable;      /* Virtual methods */

    struct gui_widget *parent;     /* Parent window/container */
    struct gui_widget *children[MAX_WIDGETS_PER_WINDOW];
    uint32_t child_count;

    /* Event handlers */
    void (*on_click)(struct gui_widget *w, int32_t x, int32_t y);
    void (*on_change)(struct gui_widget *w);
} gui_widget_t;

/* Window structure */
typedef struct {
    char title[64];
    uint32_t title_height;
    bool has_close_btn;
    bool has_minimize_btn;
    bool is_dragging;
    int32_t drag_start_x;
    int32_t drag_start_y;
} window_data_t;

/* Button structure */
typedef struct {
    char text[64];
    uint32_t text_color;
} button_data_t;

/* Label structure */
typedef struct {
    char text[256];
    uint32_t text_color;
} label_data_t;

/* TextBox structure */
typedef struct {
    char buffer[256];
    uint32_t cursor_pos;
    uint32_t selection_start;
    uint32_t selection_end;
    uint32_t text_color;
    uint32_t cursor_color;
    bool is_password;
    uint32_t max_length;
} textbox_data_t;

/* Listbox structure */
typedef struct {
    char **items;
    uint32_t item_count;
    uint32_t selected_idx;
    uint32_t visible_items;
    uint32_t scroll_offset;
    uint32_t text_color;
} listbox_data_t;

/* Color constants */
#define COLOR_WHITE         0xFFFFFFFF
#define COLOR_BLACK         0xFF000000
#define COLOR_RED           0xFFFF0000
#define COLOR_GREEN         0xFF00FF00
#define COLOR_BLUE          0xFF0000FF
#define COLOR_GRAY          0xFF808080
#define COLOR_LIGHT_GRAY    0xFFD3D3D3
#define COLOR_DARK_GRAY     0xFF404040
#define COLOR_TRANSPARENT   0x00000000

/* Default colors */
#define DEFAULT_BG_COLOR    0xFF1E1E1E  /* Dark background */
#define DEFAULT_FG_COLOR    0xFFFFFFFF  /* White text */
#define DEFAULT_BTN_COLOR   0xFF404040  /* Dark button */
#define DEFAULT_BTN_HOVER   0xFF505050  /* Slightly lighter hover */
#define DEFAULT_BTN_ACTIVE  0xFF303030  /* Darker when active */

/* ============================================
 * Widget Creation and Management
 * ============================================ */

/**
 * Initialize the GUI system
 */
void gui_init(void);

/**
 * Create a new widget
 */
gui_widget_t *gui_widget_create(widget_type_t type, const char *id);

/**
 * Destroy a widget and free resources
 */
void gui_widget_destroy(gui_widget_t *w);

/**
 * Add child widget to parent
 */
void gui_widget_add_child(gui_widget_t *parent, gui_widget_t *child);

/**
 * Remove child widget from parent
 */
void gui_widget_remove_child(gui_widget_t *parent, gui_widget_t *child);

/**
 * Set widget position and size
 */
void gui_widget_set_bounds(gui_widget_t *w, int32_t x, int32_t y,
                           uint32_t width, uint32_t height);

/**
 * Set widget visibility
 */
void gui_widget_set_visible(gui_widget_t *w, bool visible);

/**
 * Set widget enabled state
 */
void gui_widget_set_enabled(gui_widget_t *w, bool enabled);

/**
 * Mark widget for redraw
 */
void gui_widget_invalidate(gui_widget_t *w);

/**
 * Draw widget and its children
 */
void gui_widget_paint(gui_widget_t *w);

/**
 * Handle event on widget
 */
void gui_widget_handle_event(gui_widget_t *w, gui_event_t *e);

/**
 * Find widget at coordinates
 */
gui_widget_t *gui_widget_hit_test(gui_widget_t *w, int32_t x, int32_t y);

/* ============================================
 * Window Management
 * ============================================ */

/**
 * Create a new window
 */
gui_widget_t *gui_window_create(const char *title,
                                int32_t x, int32_t y,
                                uint32_t width, uint32_t height);

/**
 * Set window title
 */
void gui_window_set_title(gui_widget_t *window, const char *title);

/**
 * Bring window to front (top of Z-order)
 */
void gui_window_bring_to_front(gui_widget_t *window);

/**
 * Minimize window
 */
void gui_window_minimize(gui_widget_t *window);

/**
 * Maximize window
 */
void gui_window_maximize(gui_widget_t *window);

/**
 * Close window
 */
void gui_window_close(gui_widget_t *window);

/* ============================================
 * Button Management
 * ============================================ */

/**
 * Create a button widget
 */
gui_widget_t *gui_button_create(const char *text);

/**
 * Set button text
 */
void gui_button_set_text(gui_widget_t *button, const char *text);

/**
 * Set button click callback
 */
void gui_button_set_onclick(gui_widget_t *button,
                            void (*callback)(gui_widget_t *));

/* ============================================
 * Label Management
 * ============================================ */

/**
 * Create a label widget
 */
gui_widget_t *gui_label_create(const char *text);

/**
 * Set label text
 */
void gui_label_set_text(gui_widget_t *label, const char *text);

/* ============================================
 * TextBox Management
 * ============================================ */

/**
 * Create a textbox widget
 */
gui_widget_t *gui_textbox_create(uint32_t max_length);

/**
 * Set textbox text
 */
void gui_textbox_set_text(gui_widget_t *textbox, const char *text);

/**
 * Get textbox text
 */
const char *gui_textbox_get_text(gui_widget_t *textbox);

/**
 * Set text change callback
 */
void gui_textbox_set_on_change(gui_widget_t *textbox,
                               void (*callback)(gui_widget_t *));

/* ============================================
 * Main Event Loop
 * ============================================ */

/**
 * Process pending GUI events
 */
void gui_process_events(void);

/**
 * Redraw all dirty widgets
 */
void gui_render(void);

/**
 * Post an event to the event queue
 */
void gui_post_event(gui_event_t *event);

/**
 * Inject a mouse event
 */
void gui_inject_mouse_event(int32_t x, int32_t y,
                           mouse_button_t button, bool pressed);

/**
 * Inject a keyboard event
 */
void gui_inject_key_event(key_code_t code, char ascii, bool pressed);

#endif /* GUI_WIDGETS_H */

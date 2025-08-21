#ifndef TUI_H
#define TUI_H

#include <stdint.h>
#include <stdbool.h>

// Цвета для TUI
typedef enum {
    TUI_COLOR_BLACK = 0,
    TUI_COLOR_BLUE = 1,
    TUI_COLOR_GREEN = 2,
    TUI_COLOR_CYAN = 3,
    TUI_COLOR_RED = 4,
    TUI_COLOR_MAGENTA = 5,
    TUI_COLOR_BROWN = 6,
    TUI_COLOR_LIGHT_GRAY = 7,
    TUI_COLOR_DARK_GRAY = 8,
    TUI_COLOR_LIGHT_BLUE = 9,
    TUI_COLOR_LIGHT_GREEN = 10,
    TUI_COLOR_LIGHT_CYAN = 11,
    TUI_COLOR_LIGHT_RED = 12,
    TUI_COLOR_LIGHT_MAGENTA = 13,
    TUI_COLOR_YELLOW = 14,
    TUI_COLOR_WHITE = 15
} tui_color_t;

// Атрибуты текста
typedef enum {
    TUI_ATTR_NORMAL = 0x00,
    TUI_ATTR_BOLD = 0x01,
    TUI_ATTR_UNDERLINE = 0x02,
    TUI_ATTR_BLINK = 0x04,
    TUI_ATTR_REVERSE = 0x08
} tui_attr_t;

// Позиция на экране
typedef struct {
    uint16_t x;
    uint16_t y;
} tui_pos_t;

// Размер области
typedef struct {
    uint16_t width;
    uint16_t height;
} tui_size_t;

// Прямоугольная область
typedef struct {
    tui_pos_t pos;
    tui_size_t size;
} tui_rect_t;

// Символ с атрибутами
typedef struct {
    char character;
    tui_color_t fg_color;
    tui_color_t bg_color;
    tui_attr_t attributes;
} tui_char_t;

// Обработчик событий
typedef void (*tui_event_handler_t)(void* data);

// Типы событий
typedef enum {
    TUI_EVENT_KEY_PRESS,
    TUI_EVENT_MOUSE_CLICK,
    TUI_EVENT_WINDOW_RESIZE,
    TUI_EVENT_TIMER,
    TUI_EVENT_CUSTOM
} tui_event_type_t;

// Структура события
typedef struct tui_event {
    tui_event_type_t type;
    uint32_t timestamp;
    union {
        struct {
            uint16_t key_code;
            char character;
            bool shift;
            bool ctrl;
            bool alt;
        } key;
        struct {
            tui_pos_t pos;
            uint8_t button;
        } mouse;
        struct {
            tui_size_t new_size;
        } resize;
        struct {
            uint32_t timer_id;
        } timer;
        struct {
            uint32_t custom_id;
            void* data;
        } custom;
    } data;
} tui_event_t;

// Базовый класс для всех виджетов
typedef struct tui_widget {
    char* id;
    tui_rect_t bounds;
    bool visible;
    bool enabled;
    bool focused;
    tui_event_handler_t event_handler;
    void* user_data;
    
    // Виртуальные функции
    void (*draw)(struct tui_widget* widget);
    void (*handle_event)(struct tui_widget* widget, tui_event_t* event);
    void (*destroy)(struct tui_widget* widget);
    
    struct tui_widget* parent;
    struct tui_widget* children;
    struct tui_widget* next;
    struct tui_widget* prev;
} tui_widget_t;



// Структура окна
typedef struct tui_window {
    tui_widget_t base;
    char* title;
    tui_color_t title_color;
    tui_color_t bg_color;
    tui_color_t border_color;
    bool resizable;
    tui_size_t min_size;
} tui_window_t;

// Структура меню
typedef struct tui_menu_item {
    char* label;
    char shortcut;
    tui_event_handler_t handler;
    void* user_data;
    struct tui_menu_item* next;
} tui_menu_item_t;

typedef struct tui_menu {
    tui_widget_t base;
    char* title;
    tui_menu_item_t* items;
    uint16_t item_count;
    bool expanded;
    tui_rect_t expanded_bounds;
} tui_menu_t;

// Структура TUI системы
typedef struct {
    tui_size_t screen_size;
    tui_char_t* screen_buffer;
    tui_char_t* back_buffer;
    tui_window_t* windows;
    tui_menu_t* menus;
    tui_window_t* focused_window;
    tui_pos_t cursor_pos;
    bool cursor_visible;
    tui_color_t default_fg;
    tui_color_t default_bg;
    uint32_t tick_count;
} tui_system_t;

// Основные функции TUI системы
bool tui_system_init(void);
void tui_system_cleanup(void);
void tui_system_update(void);
void tui_system_handle_event(tui_event_t* event);

// Управление экраном
void tui_clear_screen(void);
void tui_set_cursor_pos(tui_pos_t pos);
void tui_show_cursor(bool show);
void tui_set_default_colors(tui_color_t fg, tui_color_t bg);

// Рисование
void tui_draw_char(tui_pos_t pos, char c, tui_color_t fg, tui_color_t bg);
void tui_draw_string(tui_pos_t pos, const char* str, tui_color_t fg, tui_color_t bg);
void tui_draw_rect(tui_rect_t rect, char fill_char, tui_color_t fg, tui_color_t bg);
void tui_draw_border(tui_rect_t rect, tui_color_t color);
void tui_fill_rect(tui_rect_t rect, char fill_char, tui_color_t fg, tui_color_t bg);

// Управление окнами
tui_window_t* tui_create_window(const char* id, tui_rect_t bounds, const char* title);
void tui_destroy_window(tui_window_t* window);
void tui_move_window(tui_window_t* window, tui_pos_t new_pos);
void tui_draw_window(tui_window_t* window);
void tui_handle_window_event(tui_window_t* window, tui_event_t* event);

// Управление меню
tui_menu_t* tui_create_menu(const char* id, const char* title);
void tui_destroy_menu(tui_menu_t* menu);
void tui_add_menu_item(tui_menu_t* menu, const char* label, char shortcut, tui_event_handler_t handler);
void tui_draw_menu(tui_menu_t* menu);
void tui_handle_menu_event(tui_menu_t* menu, tui_event_t* event);

// Утилиты
tui_color_t tui_make_color(uint8_t r, uint8_t g, uint8_t b);
uint16_t tui_get_screen_width(void);
uint16_t tui_get_screen_height(void);
tui_pos_t tui_get_cursor_pos(void);
uint32_t tui_get_tick_count(void);

// Обработка ввода
bool tui_is_key_pressed(uint16_t key_code);
char tui_get_char(void);
tui_event_t* tui_get_event(void);

// Дополнительные функции рисования
void tui_draw_string_at(tui_pos_t pos, const char* str, tui_color_t fg, tui_color_t bg);

#endif // TUI_H

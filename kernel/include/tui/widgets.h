#ifndef TUI_WIDGETS_H
#define TUI_WIDGETS_H

#include "tui.h"

// Кнопка
typedef struct {
    tui_widget_t base;
    char* text;
    tui_color_t text_color;
    tui_color_t bg_color;
    tui_color_t border_color;
    bool pressed;
    tui_event_handler_t click_handler;
} tui_button_t;

// Текстовое поле
typedef struct {
    tui_widget_t base;
    char* text;
    char* placeholder;
    uint16_t max_length;
    uint16_t cursor_pos;
    tui_color_t text_color;
    tui_color_t bg_color;
    tui_color_t border_color;
    bool readonly;
    tui_event_handler_t change_handler;
} tui_textbox_t;

// Список
typedef struct tui_list_item {
    char* text;
    void* data;
    bool selected;
    struct tui_list_item* next;
} tui_list_item_t;

typedef struct {
    tui_widget_t base;
    tui_list_item_t* items;
    uint16_t selected_index;
    uint16_t scroll_offset;
    uint16_t visible_items;
    tui_color_t text_color;
    tui_color_t bg_color;
    tui_color_t selection_color;
    tui_event_handler_t selection_change_handler;
} tui_list_t;

// Прогресс-бар
typedef struct {
    tui_widget_t base;
    uint8_t progress;  // 0-100
    char* label;
    tui_color_t bar_color;
    tui_color_t bg_color;
    tui_color_t text_color;
    bool show_percentage;
} tui_progressbar_t;

// Чекбокс
typedef struct {
    tui_widget_t base;
    char* text;
    bool checked;
    tui_color_t text_color;
    tui_color_t bg_color;
    tui_color_t check_color;
    tui_event_handler_t change_handler;
} tui_checkbox_t;

// Радио-кнопка
typedef struct {
    tui_widget_t base;
    char* text;
    bool checked;
    char* group;
    tui_color_t text_color;
    tui_color_t bg_color;
    tui_color_t radio_color;
    tui_event_handler_t change_handler;
} tui_radiobutton_t;

// Группа виджетов
typedef struct {
    tui_widget_t base;
    char* title;
    tui_color_t title_color;
    tui_color_t border_color;
} tui_group_t;

// Панель инструментов
typedef struct {
    tui_widget_t base;
    tui_button_t** buttons;
    uint16_t button_count;
    tui_color_t bg_color;
    tui_color_t border_color;
} tui_toolbar_t;

// Статус-бар
typedef struct {
    tui_widget_t base;
    char* left_text;
    char* center_text;
    char* right_text;
    tui_color_t text_color;
    tui_color_t bg_color;
    tui_color_t border_color;
} tui_statusbar_t;

// Функции создания виджетов
tui_button_t* tui_create_button(const char* id, tui_rect_t bounds, const char* text);
tui_textbox_t* tui_create_textbox(const char* id, tui_rect_t bounds, const char* placeholder);
tui_list_t* tui_create_list(const char* id, tui_rect_t bounds);
tui_progressbar_t* tui_create_progressbar(const char* id, tui_rect_t bounds, const char* label);
tui_checkbox_t* tui_create_checkbox(const char* id, tui_rect_t bounds, const char* text);
tui_radiobutton_t* tui_create_radiobutton(const char* id, tui_rect_t bounds, const char* text, const char* group);
tui_group_t* tui_create_group(const char* id, tui_rect_t bounds, const char* title);
tui_toolbar_t* tui_create_toolbar(const char* id, tui_rect_t bounds);
tui_statusbar_t* tui_create_statusbar(const char* id, tui_rect_t bounds);

// Функции рисования виджетов
void tui_button_draw(tui_button_t* button);
void tui_textbox_draw(tui_textbox_t* textbox);
void tui_list_draw(tui_list_t* list);
void tui_progressbar_draw(tui_progressbar_t* progressbar);
void tui_checkbox_draw(tui_checkbox_t* checkbox);
void tui_radiobutton_draw(tui_radiobutton_t* radiobutton);
void tui_group_draw(tui_group_t* group);
void tui_toolbar_draw(tui_toolbar_t* toolbar);
void tui_statusbar_draw(tui_statusbar_t* statusbar);

// Функции обработки событий виджетов
void tui_button_handle_event(tui_button_t* button, tui_event_t* event);
void tui_textbox_handle_event(tui_textbox_t* textbox, tui_event_t* event);
void tui_list_handle_event(tui_list_t* list, tui_event_t* event);
void tui_progressbar_handle_event(tui_progressbar_t* progressbar, tui_event_t* event);
void tui_checkbox_handle_event(tui_checkbox_t* checkbox, tui_event_t* event);
void tui_radiobutton_handle_event(tui_radiobutton_t* radiobutton, tui_event_t* event);
void tui_group_handle_event(tui_group_t* group, tui_event_t* event);
void tui_toolbar_handle_event(tui_toolbar_t* toolbar, tui_event_t* event);
void tui_statusbar_handle_event(tui_statusbar_t* statusbar, tui_event_t* event);

// Функции уничтожения виджетов
void tui_button_destroy(tui_button_t* button);
void tui_textbox_destroy(tui_textbox_t* textbox);
void tui_list_destroy(tui_list_t* list);
void tui_progressbar_destroy(tui_progressbar_t* progressbar);
void tui_checkbox_destroy(tui_checkbox_t* checkbox);
void tui_radiobutton_destroy(tui_radiobutton_t* radiobutton);
void tui_group_destroy(tui_group_t* group);
void tui_toolbar_destroy(tui_toolbar_t* toolbar);
void tui_statusbar_destroy(tui_statusbar_t* statusbar);

// Функции управления виджетами
void tui_widget_set_position(tui_widget_t* widget, tui_pos_t pos);
void tui_widget_set_size(tui_widget_t* widget, tui_size_t size);
void tui_widget_set_visible(tui_widget_t* widget, bool visible);
void tui_widget_set_enabled(tui_widget_t* widget, bool enabled);
void tui_widget_set_focused(tui_widget_t* widget, bool focused);
void tui_widget_add_child(tui_widget_t* parent, tui_widget_t* child);
void tui_widget_remove_child(tui_widget_t* parent, tui_widget_t* child);

// Специфичные функции для кнопки
void tui_button_set_text(tui_button_t* button, const char* text);
void tui_button_set_colors(tui_button_t* button, tui_color_t text, tui_color_t bg, tui_color_t border);

// Специфичные функции для текстового поля
void tui_textbox_set_text(tui_textbox_t* textbox, const char* text);
const char* tui_textbox_get_text(tui_textbox_t* textbox);
void tui_textbox_set_max_length(tui_textbox_t* textbox, uint16_t max_length);

// Специфичные функции для списка
void tui_list_add_item(tui_list_t* list, const char* text, void* data);
void tui_list_remove_item(tui_list_t* list, uint16_t index);
void tui_list_clear(tui_list_t* list);
void tui_list_set_selection(tui_list_t* list, uint16_t index);
uint16_t tui_list_get_selection(tui_list_t* list);

// Специфичные функции для прогресс-бара
void tui_progressbar_set_progress(tui_progressbar_t* progressbar, uint8_t progress);
void tui_progressbar_set_label(tui_progressbar_t* progressbar, const char* label);

// Специфичные функции для чекбокса
void tui_checkbox_set_checked(tui_checkbox_t* checkbox, bool checked);
bool tui_checkbox_is_checked(tui_checkbox_t* checkbox);

// Специфичные функции для радио-кнопки
void tui_radiobutton_set_checked(tui_radiobutton_t* radiobutton, bool checked);
bool tui_radiobutton_is_checked(tui_radiobutton_t* radiobutton);

// Специфичные функции для группы
void tui_group_add_widget(tui_group_t* group, tui_widget_t* widget);

// Специфичные функции для панели инструментов
void tui_toolbar_add_button(tui_toolbar_t* toolbar, tui_button_t* button);

// Специфичные функции для статус-бара
void tui_statusbar_set_left_text(tui_statusbar_t* statusbar, const char* text);
void tui_statusbar_set_center_text(tui_statusbar_t* statusbar, const char* text);
void tui_statusbar_set_right_text(tui_statusbar_t* statusbar, const char* text);

// Универсальная функция уничтожения виджета
void tui_destroy_widget(tui_widget_t* widget);

#endif // TUI_WIDGETS_H

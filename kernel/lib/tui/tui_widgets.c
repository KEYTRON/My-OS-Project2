#include "../../include/tui/widgets.h"
#include "../../include/tui/tui.h"
#include "../../lib/string.h"

// Создание кнопки
tui_button_t* tui_create_button(const char* id, tui_rect_t bounds, const char* text) {
    tui_button_t* button = (tui_button_t*)0x1001000; // Временный адрес
    if (!button) return NULL;
    
    // Инициализируем базовый виджет
    button->base.id = (char*)id;
    button->base.bounds = bounds;
    button->base.visible = true;
    button->base.enabled = true;
    button->base.focused = false;
    button->base.event_handler = NULL;
    button->base.user_data = NULL;
    button->base.parent = NULL;
    button->base.children = NULL;
    button->base.next = NULL;
    button->base.prev = NULL;
    
    // Устанавливаем функции
    button->base.draw = (void (*)(tui_widget_t*))tui_button_draw;
    button->base.handle_event = (void (*)(tui_widget_t*, tui_event_t*))tui_button_handle_event;
    button->base.destroy = (void (*)(tui_widget_t*))tui_button_destroy;
    
    // Инициализируем специфичные поля кнопки
    button->text = (char*)text;
    button->text_color = TUI_COLOR_WHITE;
    button->bg_color = TUI_COLOR_BLUE;
    button->border_color = TUI_COLOR_LIGHT_GRAY;
    button->pressed = false;
    button->click_handler = NULL;
    
    return button;
}

// Рисование кнопки
void tui_button_draw(tui_button_t* button) {
    if (!button || !button->base.visible) return;
    
    // Рисуем фон кнопки
    tui_fill_rect(button->base.bounds, ' ', button->text_color, button->bg_color);
    
    // Рисуем границу
    tui_draw_border(button->base.bounds, button->border_color);
    
    // Рисуем текст по центру
    if (button->text) {
        uint16_t text_x = button->base.bounds.pos.x + (button->base.bounds.size.width - strlen(button->text)) / 2;
        uint16_t text_y = button->base.bounds.pos.y + button->base.bounds.size.height / 2;
        
        tui_pos_t text_pos = {text_x, text_y};
        tui_draw_string(text_pos, button->text, button->text_color, button->bg_color);
    }
}

// Обработка событий кнопки
void tui_button_handle_event(tui_button_t* button, tui_event_t* event) {
    if (!button || !event) return;
    
    if (event->type == TUI_EVENT_KEY_PRESS) {
        if (event->data.key.key_code == '\n' || event->data.key.key_code == ' ') {
            // Enter или пробел - нажатие кнопки
            if (button->click_handler) {
                button->click_handler(button);
            }
        }
    }
}

// Уничтожение кнопки
void tui_button_destroy(tui_button_t* button) {
    if (!button) return;
    
    // Очищаем память
    button->base.id = NULL;
    button->text = NULL;
    button->click_handler = NULL;
}

// Создание текстового поля
tui_textbox_t* tui_create_textbox(const char* id, tui_rect_t bounds, const char* placeholder) {
    tui_textbox_t* textbox = (tui_textbox_t*)0x1002000; // Временный адрес
    if (!textbox) return NULL;
    
    // Инициализируем базовый виджет
    textbox->base.id = (char*)id;
    textbox->base.bounds = bounds;
    textbox->base.visible = true;
    textbox->base.enabled = true;
    textbox->base.focused = false;
    textbox->base.event_handler = NULL;
    textbox->base.user_data = NULL;
    textbox->base.parent = NULL;
    textbox->base.children = NULL;
    textbox->base.next = NULL;
    textbox->base.prev = NULL;
    
    // Устанавливаем функции
    textbox->base.draw = (void (*)(tui_widget_t*))tui_textbox_draw;
    textbox->base.handle_event = (void (*)(tui_widget_t*, tui_event_t*))tui_textbox_handle_event;
    textbox->base.destroy = (void (*)(tui_widget_t*))tui_textbox_destroy;
    
    // Инициализируем специфичные поля текстового поля
    textbox->text = NULL;
    textbox->placeholder = (char*)placeholder;
    textbox->max_length = 255;
    textbox->cursor_pos = 0;
    textbox->text_color = TUI_COLOR_WHITE;
    textbox->bg_color = TUI_COLOR_BLACK;
    textbox->border_color = TUI_COLOR_LIGHT_GRAY;
    textbox->readonly = false;
    textbox->change_handler = NULL;
    
    return textbox;
}

// Рисование текстового поля
void tui_textbox_draw(tui_textbox_t* textbox) {
    if (!textbox || !textbox->base.visible) return;
    
    // Рисуем фон
    tui_fill_rect(textbox->base.bounds, ' ', textbox->text_color, textbox->bg_color);
    
    // Рисуем границу
    tui_draw_border(textbox->base.bounds, textbox->border_color);
    
    // Рисуем текст или placeholder
    const char* display_text = textbox->text ? textbox->text : textbox->placeholder;
    if (display_text) {
        tui_pos_t text_pos = {
            textbox->base.bounds.pos.x + 1,
            textbox->base.bounds.pos.y + textbox->base.bounds.size.height / 2
        };
        
        // Обрезаем текст если он слишком длинный
        uint16_t max_chars = textbox->base.bounds.size.width - 2;
        if (strlen(display_text) > max_chars) {
            display_text += strlen(display_text) - max_chars;
        }
        
        tui_draw_string(text_pos, display_text, textbox->text_color, textbox->bg_color);
    }
    
    // Рисуем курсор если поле в фокусе
    if (textbox->base.focused && !textbox->readonly) {
        uint16_t cursor_x = textbox->base.bounds.pos.x + 1 + textbox->cursor_pos;
        if (cursor_x < textbox->base.bounds.pos.x + textbox->base.bounds.size.width - 1) {
            tui_pos_t cursor_pos = {cursor_x, textbox->base.bounds.pos.y + textbox->base.bounds.size.height / 2};
            tui_draw_char(cursor_pos, '_', TUI_COLOR_WHITE, TUI_COLOR_BLACK);
        }
    }
}

// Обработка событий текстового поля
void tui_textbox_handle_event(tui_textbox_t* textbox, tui_event_t* event) {
    if (!textbox || !event || textbox->readonly) return;
    
    if (event->type == TUI_EVENT_KEY_PRESS) {
        char c = event->data.key.character;

        if (c >= 32 && c <= 126) { // Печатаемые символы
            // Добавляем символ
            if (textbox->text && strlen(textbox->text) < textbox->max_length) {
                // Вставляем символ в позицию курсора
                size_t len = strlen(textbox->text);
                for (size_t i = len; i > textbox->cursor_pos; i--) {
                    textbox->text[i] = textbox->text[i - 1];
                }
                textbox->text[textbox->cursor_pos] = c;
                textbox->text[len + 1] = '\0';
                textbox->cursor_pos++;
                if (textbox->change_handler) {
                    textbox->change_handler(textbox);
                }
            }
        } else if (event->data.key.key_code == 8) { // Backspace
            // Удаляем символ
            if (textbox->text && textbox->cursor_pos > 0) {
                // Удаляем символ перед курсором
                size_t len = strlen(textbox->text);
                for (size_t i = textbox->cursor_pos - 1; i < len; i++) {
                    textbox->text[i] = textbox->text[i + 1];
                }
                textbox->text[len - 1] = '\0';
                textbox->cursor_pos--;
                if (textbox->change_handler) {
                    textbox->change_handler(textbox);
                }
            }
        } else if (event->data.key.key_code == 37) { // Left arrow
            if (textbox->cursor_pos > 0) {
                textbox->cursor_pos--;
            }
        } else if (event->data.key.key_code == 39) { // Right arrow
            if (textbox->text && textbox->cursor_pos < strlen(textbox->text)) {
                textbox->cursor_pos++;
            }
        }
    }
}

// Уничтожение текстового поля
void tui_textbox_destroy(tui_textbox_t* textbox) {
    if (!textbox) return;
    
    // Очищаем память
    textbox->base.id = NULL;
    textbox->text = NULL;
    textbox->placeholder = NULL;
    textbox->change_handler = NULL;
}

// Создание списка
tui_list_t* tui_create_list(const char* id, tui_rect_t bounds) {
    tui_list_t* list = (tui_list_t*)0x1003000; // Временный адрес
    if (!list) return NULL;
    
    // Инициализируем базовый виджет
    list->base.id = (char*)id;
    list->base.bounds = bounds;
    list->base.visible = true;
    list->base.enabled = true;
    list->base.focused = false;
    list->base.event_handler = NULL;
    list->base.user_data = NULL;
    list->base.parent = NULL;
    list->base.children = NULL;
    list->base.next = NULL;
    list->base.prev = NULL;
    
    // Устанавливаем функции
    list->base.draw = (void (*)(tui_widget_t*))tui_list_draw;
    list->base.handle_event = (void (*)(tui_widget_t*, tui_event_t*))tui_list_handle_event;
    list->base.destroy = (void (*)(tui_widget_t*))tui_list_destroy;
    
    // Инициализируем специфичные поля списка
    list->items = NULL;
    list->selected_index = 0;
    list->scroll_offset = 0;
    list->visible_items = bounds.size.height - 2; // Учитываем границы
    list->text_color = TUI_COLOR_WHITE;
    list->bg_color = TUI_COLOR_BLACK;
    list->selection_color = TUI_COLOR_BLUE;
    list->selection_change_handler = NULL;
    
    return list;
}

// Рисование списка
void tui_list_draw(tui_list_t* list) {
    if (!list || !list->base.visible) return;
    
    // Рисуем фон
    tui_fill_rect(list->base.bounds, ' ', list->text_color, list->bg_color);
    
    // Рисуем границу
    tui_draw_border(list->base.bounds, TUI_COLOR_LIGHT_GRAY);
    
    // Рисуем элементы списка
    tui_list_item_t* item = list->items;
    uint16_t item_index = 0;
    uint16_t display_index = 0;
    
    while (item && display_index < list->visible_items) {
        if (item_index >= list->scroll_offset) {
            tui_pos_t item_pos = {
                list->base.bounds.pos.x + 1,
                list->base.bounds.pos.y + 1 + display_index
            };
            
            tui_color_t fg = list->text_color;
            tui_color_t bg = list->bg_color;
            
            if (item_index == list->selected_index) {
                fg = TUI_COLOR_WHITE;
                bg = list->selection_color;
            }
            
            tui_draw_string(item_pos, item->text, fg, bg);
            display_index++;
        }
        
        item = item->next;
        item_index++;
    }
}

// Обработка событий списка
void tui_list_handle_event(tui_list_t* list, tui_event_t* event) {
    if (!list || !event) return;
    
    if (event->type == TUI_EVENT_KEY_PRESS) {
        if (event->data.key.key_code == 38) { // Up arrow
            if (list->selected_index > 0) {
                list->selected_index--;
                if (list->selection_change_handler) {
                    list->selection_change_handler(list);
                }
            }
        } else if (event->data.key.key_code == 40) { // Down arrow
            // Увеличиваем индекс выбранного элемента
            // Подсчитываем количество элементов
            uint16_t item_count = 0;
            tui_list_item_t* item = list->items;
            while (item) {
                item_count++;
                item = item->next;
            }
            if (list->selected_index < item_count - 1) {
                list->selected_index++;
                if (list->selection_change_handler) {
                    list->selection_change_handler(list);
                }
            }
        } else if (event->data.key.key_code == '\n') { // Enter
            // Выбираем элемент (сигнализируем о выборе)
            if (list->selection_change_handler) {
                list->selection_change_handler(list);
            }
        }
    }
}

// Уничтожение списка
void tui_list_destroy(tui_list_t* list) {
    if (!list) return;

    // Очищаем элементы списка
    tui_list_item_t* item = list->items;
    while (item) {
        tui_list_item_t* next = item->next;
        // Освобождаем память элемента
        item->text = NULL;
        item->data = NULL;
        item->next = NULL;
        // В системе без полноценного malloc нам нечего больше освобождать
        item = next;
    }

    // Очищаем память
    list->base.id = NULL;
    list->items = NULL;
    list->selection_change_handler = NULL;
}

// Функции управления кнопкой
void tui_button_set_text(tui_button_t* button, const char* text) {
    if (button) {
        button->text = (char*)text;
    }
}

void tui_button_set_colors(tui_button_t* button, tui_color_t text, tui_color_t bg, tui_color_t border) {
    if (button) {
        button->text_color = text;
        button->bg_color = bg;
        button->border_color = border;
    }
}

// Функции управления текстовым полем
void tui_textbox_set_text(tui_textbox_t* textbox, const char* text) {
    if (textbox) {
        textbox->text = (char*)text;
        textbox->cursor_pos = strlen(text);
    }
}

const char* tui_textbox_get_text(tui_textbox_t* textbox) {
    return textbox ? textbox->text : NULL;
}

void tui_textbox_set_max_length(tui_textbox_t* textbox, uint16_t max_length) {
    if (textbox) {
        textbox->max_length = max_length;
    }
}

// Функции управления списком
void tui_list_add_item(tui_list_t* list, const char* text, void* data) {
    if (!list || !text) return;

    // Создаём новый элемент
    tui_list_item_t* new_item = (tui_list_item_t*)0x1010000; // Временный адрес
    if (!new_item) return;

    new_item->text = (char*)text;
    new_item->data = data;
    new_item->next = NULL;

    // Добавляем в конец списка
    if (!list->items) {
        list->items = new_item;
    } else {
        tui_list_item_t* current = list->items;
        while (current->next) {
            current = current->next;
        }
        current->next = new_item;
    }
}

void tui_list_remove_item(tui_list_t* list, uint16_t index) {
    if (!list || !list->items) return;

    // Находим элемент для удаления
    if (index == 0) {
        list->items = list->items->next;
        return;
    }

    tui_list_item_t* current = list->items;
    for (uint16_t i = 0; i < index - 1 && current->next; i++) {
        current = current->next;
    }

    if (current->next) {
        current->next = current->next->next;
    }
}

void tui_list_clear(tui_list_t* list) {
    if (!list) return;

    // Удаляем все элементы
    while (list->items) {
        tui_list_item_t* next = list->items->next;
        list->items->text = NULL;
        list->items->data = NULL;
        list->items->next = NULL;
        list->items = next;
    }
    list->selected_index = 0;
    list->scroll_offset = 0;
}

void tui_list_set_selection(tui_list_t* list, uint16_t index) {
    if (list) {
        list->selected_index = index;
    }
}

uint16_t tui_list_get_selection(tui_list_t* list) {
    return list ? list->selected_index : 0;
}

// Универсальная функция уничтожения виджета
void tui_destroy_widget(tui_widget_t* widget) {
    if (!widget) return;
    
    if (widget->destroy) {
        widget->destroy(widget);
    }
}

// Функции управления виджетами
void tui_widget_set_position(tui_widget_t* widget, tui_pos_t pos) {
    if (widget) {
        widget->bounds.pos = pos;
    }
}

void tui_widget_set_size(tui_widget_t* widget, tui_size_t size) {
    if (widget) {
        widget->bounds.size = size;
    }
}

void tui_widget_set_visible(tui_widget_t* widget, bool visible) {
    if (widget) {
        widget->visible = visible;
    }
}

void tui_widget_set_enabled(tui_widget_t* widget, bool enabled) {
    if (widget) {
        widget->enabled = enabled;
    }
}

void tui_widget_set_focused(tui_widget_t* widget, bool focused) {
    if (widget) {
        widget->focused = focused;
    }
}

void tui_widget_add_child(tui_widget_t* parent, tui_widget_t* child) {
    if (!parent || !child) return;
    
    child->parent = parent;
    child->next = parent->children;
    if (parent->children) {
        parent->children->prev = child;
    }
    parent->children = child;
}

void tui_widget_remove_child(tui_widget_t* parent, tui_widget_t* child) {
    if (!parent || !child) return;
    
    if (child->prev) {
        child->prev->next = child->next;
    } else {
        parent->children = child->next;
    }
    
    if (child->next) {
        child->next->prev = child->prev;
    }
    
    child->parent = NULL;
    child->next = NULL;
    child->prev = NULL;
}

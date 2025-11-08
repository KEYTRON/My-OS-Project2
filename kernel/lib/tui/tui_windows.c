#include "../../include/tui/tui.h"
#include "../../lib/string.h"

// Создание окна
tui_window_t* tui_create_window(const char* id, tui_rect_t bounds, const char* title) {
    tui_window_t* window = (tui_window_t*)0x1000000; // Временный адрес
    if (!window) return NULL;
    
    // Инициализируем базовый виджет
    window->base.id = (char*)id;
    window->base.bounds = bounds;
    window->base.visible = true;
    window->base.enabled = true;
    window->base.focused = false;
    window->base.event_handler = NULL;
    window->base.user_data = NULL;
    window->base.parent = NULL;
    window->base.children = NULL;
    window->base.next = NULL;
    window->base.prev = NULL;
    
    // Устанавливаем функции
    window->base.draw = (void (*)(tui_widget_t*))tui_draw_window;
    window->base.handle_event = (void (*)(tui_widget_t*, tui_event_t*))tui_handle_window_event;
    window->base.destroy = (void (*)(tui_widget_t*))tui_destroy_window;
    
    // Инициализируем специфичные поля окна
    window->title = (char*)title;
    window->title_color = TUI_COLOR_WHITE;
    window->bg_color = TUI_COLOR_BLACK;
    window->border_color = TUI_COLOR_LIGHT_GRAY;
    window->resizable = true;
    window->min_size.width = 20;
    window->min_size.height = 10;
    
    return window;
}

// Рисование окна
void tui_draw_window(tui_window_t* window) {
    if (!window || !window->base.visible) return;
    
    // Рисуем фон
    tui_fill_rect(window->base.bounds, ' ', TUI_COLOR_WHITE, window->bg_color);
    
    // Рисуем границу
    tui_draw_border(window->base.bounds, window->border_color);
    
    // Рисуем заголовок
    if (window->title) {
        tui_pos_t title_pos = {window->base.bounds.pos.x + 1, window->base.bounds.pos.y};
        tui_draw_string_at(title_pos, window->title, TUI_COLOR_WHITE, window->title_color);
    }
    
    // Рисуем содержимое окна
    // Содержимое окна рисуется на основе дочерних виджетов
    // Эта функция была упрощена так как полный TUI буфер еще не реализован
}

// Обработка событий окна
void tui_handle_window_event(tui_window_t* window, tui_event_t* event) {
    if (!window || !event) return;
    
    switch (event->type) {
        case TUI_EVENT_MOUSE_CLICK:
            // Обработка кликов по окну
            break;
        case TUI_EVENT_KEY_PRESS:
            // Обработка нажатий клавиш
            break;
        default:
            break;
    }
}

// Уничтожение окна
void tui_destroy_window(tui_window_t* window) {
    if (!window) return;
    
    // Очищаем память
    window->base.id = NULL;
    window->title = NULL;
}

// Перемещение окна
void tui_move_window(tui_window_t* window, tui_pos_t new_pos) {
    if (!window) return;
    
    window->base.bounds.pos.x = new_pos.x;
    window->base.bounds.pos.y = new_pos.y;
}

// Создание меню
tui_menu_t* tui_create_menu(const char* id, const char* title) {
    tui_menu_t* menu = (tui_menu_t*)0x1001000; // Временный адрес
    if (!menu) return NULL;
    
    // Инициализируем базовый виджет
    menu->base.id = (char*)id;
    menu->base.bounds.pos.x = 0;
    menu->base.bounds.pos.y = 0;
    menu->base.bounds.size.width = 0;
    menu->base.bounds.size.height = 0;
    menu->base.visible = true;
    menu->base.enabled = true;
    menu->base.focused = false;
    menu->base.event_handler = NULL;
    menu->base.user_data = NULL;
    menu->base.parent = NULL;
    menu->base.children = NULL;
    menu->base.next = NULL;
    menu->base.prev = NULL;
    
    // Устанавливаем функции
    menu->base.draw = (void (*)(tui_widget_t*))tui_draw_menu;
    menu->base.handle_event = (void (*)(tui_widget_t*, tui_event_t*))tui_handle_menu_event;
    menu->base.destroy = (void (*)(tui_widget_t*))tui_destroy_menu;
    
    // Инициализируем специфичные поля меню
    menu->title = (char*)title;
    menu->items = NULL;
    menu->item_count = 0;
    menu->expanded = false;
    menu->expanded_bounds.pos.x = 0;
    menu->expanded_bounds.pos.y = 0;
    menu->expanded_bounds.size.width = 0;
    menu->expanded_bounds.size.height = 0;
    
    return menu;
}

// Добавление элемента меню
void tui_add_menu_item(tui_menu_t* menu, const char* label, char shortcut, tui_event_handler_t handler) {
    if (!menu || !label) return;

    // Динамическое добавление элементов меню
    if (menu->item_count < 16) { // Лимит в 16 элементов меню
        if (menu->items) {
            // Ищем последний элемент
            tui_menu_item_t* current = menu->items;
            while (current && current->next) {
                current = current->next;
            }
            // Создаём новый элемент
            tui_menu_item_t* new_item = (tui_menu_item_t*)0x1011000; // Временный адрес
            if (new_item && current) {
                new_item->label = (char*)label;
                new_item->shortcut = shortcut;
                new_item->handler = handler;
                new_item->next = NULL;
                current->next = new_item;
                menu->item_count++;
            }
        } else {
            // Первый элемент
            menu->items = (tui_menu_item_t*)0x1011000; // Временный адрес
            if (menu->items) {
                menu->items->label = (char*)label;
                menu->items->shortcut = shortcut;
                menu->items->handler = handler;
                menu->items->next = NULL;
                menu->item_count++;
            }
        }
    }
}

// Рисование меню
void tui_draw_menu(tui_menu_t* menu) {
    if (!menu || !menu->base.visible) return;
    
    // Рисуем заголовок меню
    if (menu->title) {
        tui_draw_string_at(menu->base.bounds.pos, menu->title, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
    }
    
    // Если меню развернуто, рисуем элементы
    if (menu->expanded && menu->items) {
        tui_menu_item_t* item = menu->items;
        uint16_t item_index = 0;
        
        while (item && item_index < menu->expanded_bounds.size.height) {
            // Рисуем текст элемента
            tui_pos_t item_pos = {
                menu->expanded_bounds.pos.x,
                menu->expanded_bounds.pos.y + item_index
            };
            
            if (item->shortcut) {
                // Рисуем основной текст
                tui_draw_string_at(item_pos, item->label, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
                
                // Рисуем горячую клавишу
                tui_pos_t shortcut_pos = {
                    menu->expanded_bounds.pos.x + 20, // Смещение для горячей клавиши
                    menu->expanded_bounds.pos.y + item_index
                };
                char shortcut_text[8] = "[ ]";
                shortcut_text[1] = item->shortcut;
                tui_draw_string_at(shortcut_pos, shortcut_text, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
            } else {
                tui_draw_string_at(item_pos, item->label, TUI_COLOR_WHITE, TUI_COLOR_BLACK);
            }
            
            item = item->next;
            item_index++;
        }
    }
}

// Обработка событий меню
void tui_handle_menu_event(tui_menu_t* menu, tui_event_t* event) {
    if (!menu || !event) return;
    
    switch (event->type) {
        case TUI_EVENT_MOUSE_CLICK:
            // Обработка кликов по меню
            break;
        case TUI_EVENT_KEY_PRESS:
            // Обработка нажатий клавиш
            break;
        default:
            break;
    }
}

// Уничтожение меню
void tui_destroy_menu(tui_menu_t* menu) {
    if (!menu) return;
    
    // Очищаем память
    menu->base.id = NULL;
    menu->title = NULL;
    menu->items = NULL;
}

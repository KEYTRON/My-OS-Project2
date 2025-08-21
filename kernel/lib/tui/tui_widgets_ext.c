#include "../../include/tui/widgets.h"
#include "../../include/tui/tui.h"
#include "../../lib/string.h"

// Создание прогресс-бара
tui_progressbar_t* tui_create_progressbar(const char* id, tui_rect_t bounds, const char* label) {
    tui_progressbar_t* progressbar = (tui_progressbar_t*)0x100A000; // Временный адрес
    if (!progressbar) return NULL;
    
    // Инициализируем базовый виджет
    progressbar->base.id = (char*)id;
    progressbar->base.bounds = bounds;
    progressbar->base.visible = true;
    progressbar->base.enabled = true;
    progressbar->base.focused = false;
    progressbar->base.event_handler = NULL;
    progressbar->base.user_data = NULL;
    progressbar->base.parent = NULL;
    progressbar->base.children = NULL;
    progressbar->base.next = NULL;
    progressbar->base.prev = NULL;
    
    // Устанавливаем функции
    progressbar->base.draw = (void (*)(tui_widget_t*))tui_progressbar_draw;
    progressbar->base.handle_event = (void (*)(tui_widget_t*, tui_event_t*))tui_progressbar_handle_event;
    progressbar->base.destroy = (void (*)(tui_widget_t*))tui_progressbar_destroy;
    
    // Инициализируем специфичные поля прогресс-бара
    progressbar->progress = 0;
    progressbar->label = (char*)label;
    progressbar->bar_color = TUI_COLOR_GREEN;
    progressbar->bg_color = TUI_COLOR_DARK_GRAY;
    progressbar->text_color = TUI_COLOR_WHITE;
    progressbar->show_percentage = true;
    
    return progressbar;
}

// Рисование прогресс-бара
void tui_progressbar_draw(tui_progressbar_t* progressbar) {
    if (!progressbar || !progressbar->base.visible) return;
    
    // Рисуем фон
    tui_fill_rect(progressbar->base.bounds, ' ', progressbar->text_color, progressbar->bg_color);
    
    // Рисуем границу
    tui_draw_border(progressbar->base.bounds, TUI_COLOR_LIGHT_GRAY);
    
    // Рисуем метку
    if (progressbar->label) {
        tui_pos_t label_pos = {
            progressbar->base.bounds.pos.x + 1,
            progressbar->base.bounds.pos.y
        };
        tui_draw_string(label_pos, progressbar->label, progressbar->text_color, progressbar->bg_color);
    }
    
    // Рисуем полосу прогресса
    uint16_t bar_width = progressbar->base.bounds.size.width - 2;
    uint16_t filled_width = (bar_width * progressbar->progress) / 100;
    
    tui_rect_t bar_rect = {
        .pos = {progressbar->base.bounds.pos.x + 1, progressbar->base.bounds.pos.y + 1},
        .size = {filled_width, progressbar->base.bounds.size.height - 2}
    };
    
    if (filled_width > 0) {
        tui_fill_rect(bar_rect, ' ', TUI_COLOR_WHITE, progressbar->bar_color);
    }
    
    // Рисуем процент если нужно
    if (progressbar->show_percentage) {
        char percent_text[8];
        // Простое форматирование процентов
        if (progressbar->progress < 10) {
            percent_text[0] = ' ';
            percent_text[1] = '0' + progressbar->progress;
        } else if (progressbar->progress < 100) {
            percent_text[0] = '0' + (progressbar->progress / 10);
            percent_text[1] = '0' + (progressbar->progress % 10);
        } else {
            percent_text[0] = '1';
            percent_text[1] = '0';
            percent_text[2] = '0';
        }
        percent_text[3] = '%';
        percent_text[4] = '\0';
        
        uint16_t text_x = progressbar->base.bounds.pos.x + (progressbar->base.bounds.size.width - strlen(percent_text)) / 2;
        tui_pos_t percent_pos = {
            text_x,
            progressbar->base.bounds.pos.y + progressbar->base.bounds.size.height / 2
        };
        
        tui_draw_string(percent_pos, percent_text, progressbar->text_color, progressbar->bg_color);
    }
}

// Обработка событий прогресс-бара
void tui_progressbar_handle_event(tui_progressbar_t* progressbar, tui_event_t* event) {
    // Прогресс-бар не обрабатывает события
    (void)progressbar;
    (void)event;
}

// Уничтожение прогресс-бара
void tui_progressbar_destroy(tui_progressbar_t* progressbar) {
    if (!progressbar) return;
    
    // Очищаем память
    progressbar->base.id = NULL;
    progressbar->label = NULL;
}

// Создание чекбокса
tui_checkbox_t* tui_create_checkbox(const char* id, tui_rect_t bounds, const char* text) {
    tui_checkbox_t* checkbox = (tui_checkbox_t*)0x100B000; // Временный адрес
    if (!checkbox) return NULL;
    
    // Инициализируем базовый виджет
    checkbox->base.id = (char*)id;
    checkbox->base.bounds = bounds;
    checkbox->base.visible = true;
    checkbox->base.enabled = true;
    checkbox->base.focused = false;
    checkbox->base.event_handler = NULL;
    checkbox->base.user_data = NULL;
    checkbox->base.parent = NULL;
    checkbox->base.children = NULL;
    checkbox->base.next = NULL;
    checkbox->base.prev = NULL;
    
    // Устанавливаем функции
    checkbox->base.draw = (void (*)(tui_widget_t*))tui_checkbox_draw;
    checkbox->base.handle_event = (void (*)(tui_widget_t*, tui_event_t*))tui_checkbox_handle_event;
    checkbox->base.destroy = (void (*)(tui_widget_t*))tui_checkbox_destroy;
    
    // Инициализируем специфичные поля чекбокса
    checkbox->text = (char*)text;
    checkbox->checked = false;
    checkbox->text_color = TUI_COLOR_WHITE;
    checkbox->bg_color = TUI_COLOR_BLACK;
    checkbox->check_color = TUI_COLOR_GREEN;
    checkbox->change_handler = NULL;
    
    return checkbox;
}

// Рисование чекбокса
void tui_checkbox_draw(tui_checkbox_t* checkbox) {
    if (!checkbox || !checkbox->base.visible) return;
    
    // Рисуем фон
    tui_fill_rect(checkbox->base.bounds, ' ', checkbox->text_color, checkbox->bg_color);
    
    // Рисуем рамку чекбокса
    tui_rect_t check_rect = {
        .pos = {checkbox->base.bounds.pos.x, checkbox->base.bounds.pos.y + checkbox->base.bounds.size.height / 2 - 1},
        .size = {3, 3}
    };
    
    tui_draw_border(check_rect, TUI_COLOR_LIGHT_GRAY);
    
    // Рисуем галочку если отмечен
    if (checkbox->checked) {
        tui_pos_t check_pos = {
            checkbox->base.bounds.pos.x + 1,
            checkbox->base.bounds.pos.y + checkbox->base.bounds.size.height / 2
        };
        tui_draw_char(check_pos, 'X', checkbox->check_color, checkbox->bg_color);
    }
    
    // Рисуем текст
    if (checkbox->text) {
        tui_pos_t text_pos = {
            checkbox->base.bounds.pos.x + 5,
            checkbox->base.bounds.pos.y + checkbox->base.bounds.size.height / 2
        };
        tui_draw_string(text_pos, checkbox->text, checkbox->text_color, checkbox->bg_color);
    }
}

// Обработка событий чекбокса
void tui_checkbox_handle_event(tui_checkbox_t* checkbox, tui_event_t* event) {
    if (!checkbox || !event) return;
    
    if (event->type == TUI_EVENT_KEY_PRESS) {
        if (event->data.key.key_code == ' ' || event->data.key.key_code == '\n') {
            // Пробел или Enter - переключение состояния
            checkbox->checked = !checkbox->checked;
            
            if (checkbox->change_handler) {
                checkbox->change_handler(checkbox);
            }
        }
    }
}

// Уничтожение чекбокса
void tui_checkbox_destroy(tui_checkbox_t* checkbox) {
    if (!checkbox) return;
    
    // Очищаем память
    checkbox->base.id = NULL;
    checkbox->text = NULL;
    checkbox->change_handler = NULL;
}

// Создание радио-кнопки
tui_radiobutton_t* tui_create_radiobutton(const char* id, tui_rect_t bounds, const char* text, const char* group) {
    tui_radiobutton_t* radiobutton = (tui_radiobutton_t*)0x100C000; // Временный адрес
    if (!radiobutton) return NULL;
    
    // Инициализируем базовый виджет
    radiobutton->base.id = (char*)id;
    radiobutton->base.bounds = bounds;
    radiobutton->base.visible = true;
    radiobutton->base.enabled = true;
    radiobutton->base.focused = false;
    radiobutton->base.event_handler = NULL;
    radiobutton->base.user_data = NULL;
    radiobutton->base.parent = NULL;
    radiobutton->base.children = NULL;
    radiobutton->base.next = NULL;
    radiobutton->base.prev = NULL;
    
    // Устанавливаем функции
    radiobutton->base.draw = (void (*)(tui_widget_t*))tui_radiobutton_draw;
    radiobutton->base.handle_event = (void (*)(tui_widget_t*, tui_event_t*))tui_radiobutton_handle_event;
    radiobutton->base.destroy = (void (*)(tui_widget_t*))tui_radiobutton_destroy;
    
    // Инициализируем специфичные поля радио-кнопки
    radiobutton->text = (char*)text;
    radiobutton->checked = false;
    radiobutton->group = (char*)group;
    radiobutton->text_color = TUI_COLOR_WHITE;
    radiobutton->bg_color = TUI_COLOR_BLACK;
    radiobutton->radio_color = TUI_COLOR_BLUE;
    radiobutton->change_handler = NULL;
    
    return radiobutton;
}

// Рисование радио-кнопки
void tui_radiobutton_draw(tui_radiobutton_t* radiobutton) {
    if (!radiobutton || !radiobutton->base.visible) return;
    
    // Рисуем фон
    tui_fill_rect(radiobutton->base.bounds, ' ', radiobutton->text_color, radiobutton->bg_color);
    
    // Рисуем круг радио-кнопки
    tui_pos_t radio_pos = {
        radiobutton->base.bounds.pos.x,
        radiobutton->base.bounds.pos.y + radiobutton->base.bounds.size.height / 2
    };
    
    // Рисуем круг (используем символ 'O')
    tui_draw_char(radio_pos, 'O', TUI_COLOR_LIGHT_GRAY, radiobutton->bg_color);
    
    // Рисуем точку если отмечен
    if (radiobutton->checked) {
        tui_draw_char(radio_pos, 'O', radiobutton->radio_color, radiobutton->bg_color);
        tui_pos_t dot_pos = {radio_pos.x, radio_pos.y};
        tui_draw_char(dot_pos, '.', TUI_COLOR_WHITE, radiobutton->radio_color);
    }
    
    // Рисуем текст
    if (radiobutton->text) {
        tui_pos_t text_pos = {
            radiobutton->base.bounds.pos.x + 3,
            radiobutton->base.bounds.pos.y + radiobutton->base.bounds.size.height / 2
        };
        tui_draw_string(text_pos, radiobutton->text, radiobutton->text_color, radiobutton->bg_color);
    }
}

// Обработка событий радио-кнопки
void tui_radiobutton_handle_event(tui_radiobutton_t* radiobutton, tui_event_t* event) {
    if (!radiobutton || !event) return;
    
    if (event->type == TUI_EVENT_KEY_PRESS) {
        if (event->data.key.key_code == ' ' || event->data.key.key_code == '\n') {
            // Пробел или Enter - выбор радио-кнопки
            // TODO: Реализовать снятие отметки с других радио-кнопок в группе
            
            radiobutton->checked = true;
            
            if (radiobutton->change_handler) {
                radiobutton->change_handler(radiobutton);
            }
        }
    }
}

// Уничтожение радио-кнопки
void tui_radiobutton_destroy(tui_radiobutton_t* radiobutton) {
    if (!radiobutton) return;
    
    // Очищаем память
    radiobutton->base.id = NULL;
    radiobutton->text = NULL;
    radiobutton->group = NULL;
    radiobutton->change_handler = NULL;
}

// Создание группы виджетов
tui_group_t* tui_create_group(const char* id, tui_rect_t bounds, const char* title) {
    tui_group_t* group = (tui_group_t*)0x100D000; // Временный адрес
    if (!group) return NULL;
    
    // Инициализируем базовый виджет
    group->base.id = (char*)id;
    group->base.bounds = bounds;
    group->base.visible = true;
    group->base.enabled = true;
    group->base.focused = false;
    group->base.event_handler = NULL;
    group->base.user_data = NULL;
    group->base.parent = NULL;
    group->base.children = NULL;
    group->base.next = NULL;
    group->base.prev = NULL;
    
    // Устанавливаем функции
    group->base.draw = (void (*)(tui_widget_t*))tui_group_draw;
    group->base.handle_event = (void (*)(tui_widget_t*, tui_event_t*))tui_group_handle_event;
    group->base.destroy = (void (*)(tui_widget_t*))tui_group_destroy;
    
    // Инициализируем специфичные поля группы
    group->title = (char*)title;
    group->title_color = TUI_COLOR_WHITE;
    group->border_color = TUI_COLOR_LIGHT_GRAY;
    
    return group;
}

// Рисование группы виджетов
void tui_group_draw(tui_group_t* group) {
    if (!group || !group->base.visible) return;
    
    // Рисуем границу группы
    tui_draw_border(group->base.bounds, group->border_color);
    
    // Рисуем заголовок
    if (group->title) {
        tui_pos_t title_pos = {
            group->base.bounds.pos.x + 1,
            group->base.bounds.pos.y
        };
        tui_draw_string(title_pos, group->title, group->title_color, TUI_COLOR_BLACK);
    }
    
    // Рисуем дочерние виджеты
    tui_widget_t* child = group->base.children;
    while (child) {
        if (child->draw) {
            child->draw(child);
        }
        child = child->next;
    }
}

// Обработка событий группы виджетов
void tui_group_handle_event(tui_group_t* group, tui_event_t* event) {
    if (!group || !event) return;
    
    // Передаем события дочерним виджетам
    tui_widget_t* child = group->base.children;
    while (child) {
        if (child->handle_event) {
            child->handle_event(child, event);
        }
        child = child->next;
    }
}

// Уничтожение группы виджетов
void tui_group_destroy(tui_group_t* group) {
    if (!group) return;
    
    // Уничтожаем дочерние виджеты
    tui_widget_t* child = group->base.children;
    while (child) {
        tui_widget_t* next = child->next;
        if (child->destroy) {
            child->destroy(child);
        }
        child = next;
    }
    
    // Очищаем память
    group->base.id = NULL;
    group->title = NULL;
}

// Добавление виджета в группу
void tui_group_add_widget(tui_group_t* group, tui_widget_t* widget) {
    if (!group || !widget) return;
    
    tui_widget_add_child((tui_widget_t*)group, widget);
}

// Функции управления виджетами
void tui_progressbar_set_progress(tui_progressbar_t* progressbar, uint8_t progress) {
    if (progressbar) {
        progressbar->progress = progress > 100 ? 100 : progress;
    }
}

void tui_progressbar_set_label(tui_progressbar_t* progressbar, const char* label) {
    if (progressbar) {
        progressbar->label = (char*)label;
    }
}

void tui_checkbox_set_checked(tui_checkbox_t* checkbox, bool checked) {
    if (checkbox) {
        checkbox->checked = checked;
    }
}

bool tui_checkbox_is_checked(tui_checkbox_t* checkbox) {
    return checkbox ? checkbox->checked : false;
}

void tui_radiobutton_set_checked(tui_radiobutton_t* radiobutton, bool checked) {
    if (radiobutton) {
        radiobutton->checked = checked;
    }
}

bool tui_radiobutton_is_checked(tui_radiobutton_t* radiobutton) {
    return radiobutton ? radiobutton->checked : false;
}

// Создание панели инструментов
tui_toolbar_t* tui_create_toolbar(const char* id, tui_rect_t bounds) {
    tui_toolbar_t* toolbar = (tui_toolbar_t*)0x1008000; // Временный адрес
    if (!toolbar) return NULL;
    
    // Инициализируем базовый виджет
    toolbar->base.id = (char*)id;
    toolbar->base.bounds = bounds;
    toolbar->base.visible = true;
    toolbar->base.enabled = true;
    toolbar->base.focused = false;
    toolbar->base.event_handler = NULL;
    toolbar->base.user_data = NULL;
    toolbar->base.parent = NULL;
    toolbar->base.children = NULL;
    toolbar->base.next = NULL;
    toolbar->base.prev = NULL;
    
    // Устанавливаем функции
    toolbar->base.draw = (void (*)(tui_widget_t*))tui_toolbar_draw;
    toolbar->base.handle_event = (void (*)(tui_widget_t*, tui_event_t*))tui_toolbar_handle_event;
    toolbar->base.destroy = (void (*)(tui_widget_t*))tui_toolbar_destroy;
    
    // Инициализируем специфичные поля панели инструментов
    toolbar->buttons = NULL;
    toolbar->button_count = 0;
    toolbar->bg_color = TUI_COLOR_DARK_GRAY;
    toolbar->border_color = TUI_COLOR_LIGHT_GRAY;
    
    return toolbar;
}

// Рисование панели инструментов
void tui_toolbar_draw(tui_toolbar_t* toolbar) {
    if (!toolbar || !toolbar->base.visible) return;
    
    // Рисуем фон
    tui_fill_rect(toolbar->base.bounds, ' ', TUI_COLOR_WHITE, toolbar->bg_color);
    
    // Рисуем границу
    tui_draw_border(toolbar->base.bounds, toolbar->border_color);
    
    // Рисуем кнопки
    for (uint16_t i = 0; i < toolbar->button_count; i++) {
        if (toolbar->buttons[i]) {
            tui_button_draw(toolbar->buttons[i]);
        }
    }
}

// Обработка событий панели инструментов
void tui_toolbar_handle_event(tui_toolbar_t* toolbar, tui_event_t* event) {
    if (!toolbar || !event) return;
    
    // Передаем события кнопкам
    for (uint16_t i = 0; i < toolbar->button_count; i++) {
        if (toolbar->buttons[i]) {
            tui_button_handle_event(toolbar->buttons[i], event);
        }
    }
}

// Уничтожение панели инструментов
void tui_toolbar_destroy(tui_toolbar_t* toolbar) {
    if (!toolbar) return;
    
    // Очищаем память
    toolbar->base.id = NULL;
    toolbar->buttons = NULL;
}

// Добавление кнопки в панель инструментов
void tui_toolbar_add_button(tui_toolbar_t* toolbar, tui_button_t* button) {
    if (!toolbar || !button) return;
    
    // TODO: Реализовать динамическое добавление кнопок
    // Пока просто увеличиваем счетчик
    toolbar->button_count++;
}

// Создание статус-бара
tui_statusbar_t* tui_create_statusbar(const char* id, tui_rect_t bounds) {
    tui_statusbar_t* statusbar = (tui_statusbar_t*)0x1009000; // Временный адрес
    if (!statusbar) return NULL;
    
    // Инициализируем базовый виджет
    statusbar->base.id = (char*)id;
    statusbar->base.bounds = bounds;
    statusbar->base.visible = true;
    statusbar->base.enabled = true;
    statusbar->base.focused = false;
    statusbar->base.event_handler = NULL;
    statusbar->base.user_data = NULL;
    statusbar->base.parent = NULL;
    statusbar->base.children = NULL;
    statusbar->base.next = NULL;
    statusbar->base.prev = NULL;
    
    // Устанавливаем функции
    statusbar->base.draw = (void (*)(tui_widget_t*))tui_statusbar_draw;
    statusbar->base.handle_event = (void (*)(tui_widget_t*, tui_event_t*))tui_statusbar_handle_event;
    statusbar->base.destroy = (void (*)(tui_widget_t*))tui_statusbar_destroy;
    
    // Инициализируем специфичные поля статус-бара
    statusbar->left_text = NULL;
    statusbar->center_text = NULL;
    statusbar->right_text = NULL;
    statusbar->text_color = TUI_COLOR_WHITE;
    statusbar->bg_color = TUI_COLOR_DARK_GRAY;
    statusbar->border_color = TUI_COLOR_LIGHT_GRAY;
    
    return statusbar;
}

// Рисование статус-бара
void tui_statusbar_draw(tui_statusbar_t* statusbar) {
    if (!statusbar || !statusbar->base.visible) return;
    
    // Рисуем фон
    tui_fill_rect(statusbar->base.bounds, ' ', statusbar->text_color, statusbar->bg_color);
    
    // Рисуем границу
    tui_draw_border(statusbar->base.bounds, statusbar->border_color);
    
    // Рисуем левый текст
    if (statusbar->left_text) {
        tui_pos_t left_pos = {
            statusbar->base.bounds.pos.x + 1,
            statusbar->base.bounds.pos.y + statusbar->base.bounds.size.height / 2
        };
        tui_draw_string(left_pos, statusbar->left_text, statusbar->text_color, statusbar->bg_color);
    }
    
    // Рисуем центральный текст
    if (statusbar->center_text) {
        uint16_t center_x = statusbar->base.bounds.pos.x + (statusbar->base.bounds.size.width - strlen(statusbar->center_text)) / 2;
        tui_pos_t center_pos = {
            center_x,
            statusbar->base.bounds.pos.y + statusbar->base.bounds.size.height / 2
        };
        tui_draw_string(center_pos, statusbar->center_text, statusbar->text_color, statusbar->bg_color);
    }
    
    // Рисуем правый текст
    if (statusbar->right_text) {
        uint16_t right_x = statusbar->base.bounds.pos.x + statusbar->base.bounds.size.width - strlen(statusbar->right_text) - 1;
        tui_pos_t right_pos = {
            right_x,
            statusbar->base.bounds.pos.y + statusbar->base.bounds.size.height / 2
        };
        tui_draw_string(right_pos, statusbar->right_text, statusbar->text_color, statusbar->bg_color);
    }
}

// Обработка событий статус-бара
void tui_statusbar_handle_event(tui_statusbar_t* statusbar, tui_event_t* event) {
    // Статус-бар не обрабатывает события
    (void)statusbar;
    (void)event;
}

// Уничтожение статус-бара
void tui_statusbar_destroy(tui_statusbar_t* statusbar) {
    if (!statusbar) return;
    
    // Очищаем память
    statusbar->base.id = NULL;
    statusbar->left_text = NULL;
    statusbar->center_text = NULL;
    statusbar->right_text = NULL;
}

// Установка текста статус-бара
void tui_statusbar_set_left_text(tui_statusbar_t* statusbar, const char* text) {
    if (statusbar) {
        statusbar->left_text = (char*)text;
    }
}

void tui_statusbar_set_center_text(tui_statusbar_t* statusbar, const char* text) {
    if (statusbar) {
        statusbar->center_text = (char*)text;
    }
}

void tui_statusbar_set_right_text(tui_statusbar_t* statusbar, const char* text) {
    if (statusbar) {
        statusbar->right_text = (char*)text;
    }
}

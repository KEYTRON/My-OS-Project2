#include "../../include/tui/tui_demo.h"
#include "../../include/tui/tui.h"
#include "../../include/tui/widgets.h"
#include "../../lib/printf.h"
#include "../drivers/serial.h"

// Создание демонстрационного приложения
tui_demo_app_t* tui_demo_create(void) {
    tui_demo_app_t* app = (tui_demo_app_t*)0x1004000; // Временный адрес
    if (!app) return NULL;
    
    // Инициализируем состояние
    app->running = false;
    app->progress = 0;
    app->start_time = 0;
    app->last_update = 0;
    
    // Создаем интерфейс
    tui_demo_create_interface(app);
    
    return app;
}

// Уничтожение демонстрационного приложения
void tui_demo_destroy(tui_demo_app_t* app) {
    if (!app) return;
    
    // Очищаем интерфейс
    tui_demo_cleanup(app);
    
    // Очищаем память
    app->main_window = NULL;
    app->toolbar = NULL;
    app->statusbar = NULL;
    app->control_group = NULL;
    app->display_group = NULL;
    app->start_button = NULL;
    app->stop_button = NULL;
    app->reset_button = NULL;
    app->auto_checkbox = NULL;
    app->mode1_radio = NULL;
    app->mode2_radio = NULL;
    app->mode3_radio = NULL;
    app->progress_bar = NULL;
    app->log_list = NULL;
    app->input_box = NULL;
}

// Запуск демонстрации
void tui_demo_run(tui_demo_app_t* app) {
    if (!app) return;
    
    printf("Запуск TUI демонстрации...\n");
    serial_write_string("Starting TUI demo...\n");
    
    // Показываем главное окно
    if (app->main_window) {
        // Окно уже видимо по умолчанию
        // Фокус устанавливается автоматически
    }
    
    // Основной цикл демонстрации
    app->running = true;
    app->start_time = tui_get_tick_count();
    
    while (app->running) {
        // Обновляем TUI систему
        tui_system_update();
        
        // Обновляем демонстрацию
        tui_demo_update(app);
        
        // Обрабатываем события
        tui_event_t* event = tui_get_event();
        if (event) {
            tui_demo_handle_keyboard(app, event);
            tui_demo_handle_mouse(app, event);
            tui_system_handle_event(event);
        }
        
        // Небольшая задержка
        for (volatile int i = 0; i < 1000000; i++);
    }
    
    printf("TUI демонстрация завершена.\n");
    serial_write_string("TUI demo finished.\n");
}

// Очистка демонстрации
void tui_demo_cleanup(tui_demo_app_t* app) {
    if (!app) return;
    
    // Скрываем все окна
    if (app->main_window) {
        // Окно скрывается автоматически при уничтожении
    }
    
    // Останавливаем демонстрацию
    app->running = false;
}

// Обработчики событий
void tui_demo_start_button_handler(void* data) {
    tui_demo_app_t* app = (tui_demo_app_t*)data;
    if (!app) return;
    
    app->running = true;
    app->start_time = tui_get_tick_count();
    app->progress = 0;
    
    tui_demo_add_log_message(app, "Демонстрация запущена");
    
    // Обновляем статус кнопок
    if (app->start_button) {
        tui_widget_set_enabled((tui_widget_t*)app->start_button, false);
    }
    if (app->stop_button) {
        tui_widget_set_enabled((tui_widget_t*)app->stop_button, true);
    }
}

void tui_demo_stop_button_handler(void* data) {
    tui_demo_app_t* app = (tui_demo_app_t*)data;
    if (!app) return;
    
    app->running = false;
    tui_demo_add_log_message(app, "Демонстрация остановлена");
    
    // Обновляем статус кнопок
    if (app->start_button) {
        tui_widget_set_enabled((tui_widget_t*)app->start_button, true);
    }
    if (app->stop_button) {
        tui_widget_set_enabled((tui_widget_t*)app->stop_button, false);
    }
}

void tui_demo_reset_button_handler(void* data) {
    tui_demo_app_t* app = (tui_demo_app_t*)data;
    if (!app) return;
    
    app->progress = 0;
    if (app->progress_bar) {
        tui_progressbar_set_progress(app->progress_bar, 0);
    }
    
    tui_demo_add_log_message(app, "Прогресс сброшен");
}

void tui_demo_auto_checkbox_handler(void* data) {
    tui_demo_app_t* app = (tui_demo_app_t*)data;
    if (!app || !app->auto_checkbox) return;

    // Реализуем автоматический режим
    if (app->auto_checkbox->checked) {
        tui_demo_add_log_message(app, "Автоматический режим включен");
    } else {
        tui_demo_add_log_message(app, "Автоматический режим отключен");
    }
}

void tui_demo_mode_radio_handler(void* data) {
    tui_demo_app_t* app = (tui_demo_app_t*)data;
    if (!app) return;

    // Реализуем изменение режима
    if (app->mode1_radio && app->mode1_radio->checked) {
        tui_demo_add_log_message(app, "Режим: Режим 1");
    } else if (app->mode2_radio && app->mode2_radio->checked) {
        tui_demo_add_log_message(app, "Режим: Режим 2");
    } else if (app->mode3_radio && app->mode3_radio->checked) {
        tui_demo_add_log_message(app, "Режим: Режим 3");
    }
}

void tui_demo_input_change_handler(void* data) {
    tui_demo_app_t* app = (tui_demo_app_t*)data;
    if (!app || !app->input_box) return;

    // Реализуем обработку изменения ввода
    tui_demo_add_log_message(app, "Ввод изменен");
}

// Обновление демонстрации
void tui_demo_update(tui_demo_app_t* app) {
    if (!app || !app->running) return;
    
    uint32_t current_time = tui_get_tick_count();
    
    // Обновляем каждые 100 тиков
    if (current_time - app->last_update >= 100) {
        tui_demo_update_progress(app);
        app->last_update = current_time;
    }
}

// Обновление прогресса
void tui_demo_update_progress(tui_demo_app_t* app) {
    if (!app) return;
    
    app->progress++;
    if (app->progress > 100) {
        app->progress = 0;
        tui_demo_add_log_message(app, "Прогресс завершен, начинаем заново");
    }
    
    if (app->progress_bar) {
        tui_progressbar_set_progress(app->progress_bar, app->progress);
    }
    
    // Обновляем статус-бар
    if (app->statusbar) {
        char status_text[64];
        printf(status_text, "Прогресс: %d%%", app->progress);
        tui_statusbar_set_center_text(app->statusbar, status_text);
    }
}

// Добавление сообщения в лог
void tui_demo_add_log_message(tui_demo_app_t* app, const char* message) {
    if (!app || !message) return;

    // Реализуем добавление сообщения в список логов
    if (app->log_list) {
        tui_list_add_item(app->log_list, message, NULL);
    }

    // Выводим в serial для отладки
    serial_write_string("TUI Demo: ");
    serial_write_string(message);
    serial_write_string("\n");
}

// Создание интерфейса
void tui_demo_create_interface(tui_demo_app_t* app) {
    if (!app) return;
    
    // Создаем главное окно
    tui_size_t screen_size = {tui_get_screen_width(), tui_get_screen_height()};
    tui_rect_t main_bounds = tui_demo_calculate_layout(screen_size);
    
    app->main_window = tui_create_window("main_window", main_bounds, "MyOS TUI Demo");
    if (!app->main_window) return;
    
    // Создаем компоненты интерфейса
    tui_demo_create_toolbar(app);
    tui_demo_create_control_panel(app);
    tui_demo_create_display_panel(app);
    tui_demo_create_statusbar(app);
    
    // Центрируем окно
    tui_demo_center_window(app->main_window);
}

// Создание панели инструментов
void tui_demo_create_toolbar(tui_demo_app_t* app) {
    if (!app) return;
    
    tui_rect_t toolbar_bounds = {
        .pos = {1, 1},
        .size = {78, 3}
    };
    
    app->toolbar = tui_create_toolbar("main_toolbar", toolbar_bounds);
    if (!app->toolbar) return;
    
    // Создаем кнопки панели инструментов
    tui_rect_t start_button_bounds = {.pos = {2, 2}, .size = {8, 1}};
    app->start_button = tui_create_button("start_btn", start_button_bounds, "Старт");
    if (app->start_button) {
        app->start_button->click_handler = tui_demo_start_button_handler;
        app->start_button->base.user_data = app;
        tui_toolbar_add_button(app->toolbar, app->start_button);
    }
    
    tui_rect_t stop_button_bounds = {.pos = {12, 2}, .size = {8, 1}};
    app->stop_button = tui_create_button("stop_btn", stop_button_bounds, "Стоп");
    if (app->stop_button) {
        app->stop_button->click_handler = tui_demo_stop_button_handler;
        app->stop_button->base.user_data = app;
        app->stop_button->bg_color = TUI_COLOR_RED;
        tui_toolbar_add_button(app->toolbar, app->stop_button);
        tui_widget_set_enabled((tui_widget_t*)app->stop_button, false);
    }
    
    tui_rect_t reset_button_bounds = {.pos = {22, 2}, .size = {8, 1}};
    app->reset_button = tui_create_button("reset_btn", reset_button_bounds, "Сброс");
    if (app->reset_button) {
        app->reset_button->click_handler = tui_demo_reset_button_handler;
        app->reset_button->base.user_data = app;
        app->reset_button->bg_color = TUI_COLOR_YELLOW;
        tui_toolbar_add_button(app->toolbar, app->reset_button);
    }
}

// Создание панели управления
void tui_demo_create_control_panel(tui_demo_app_t* app) {
    if (!app) return;
    
    tui_rect_t control_bounds = {
        .pos = {1, 5},
        .size = {25, 15}
    };
    
    app->control_group = tui_create_group("control_group", control_bounds, "Управление");
    if (!app->control_group) return;
    
    // Чекбокс автоматического режима
    tui_rect_t checkbox_bounds = {.pos = {2, 7}, .size = {20, 1}};
    app->auto_checkbox = tui_create_checkbox("auto_checkbox", checkbox_bounds, "Автоматический режим");
    if (app->auto_checkbox) {
        app->auto_checkbox->change_handler = tui_demo_auto_checkbox_handler;
        app->auto_checkbox->base.user_data = app;
        tui_group_add_widget(app->control_group, (tui_widget_t*)app->auto_checkbox);
    }

    // Радио-кнопки режимов
    tui_rect_t radio1_bounds = {.pos = {2, 9}, .size = {20, 1}};
    app->mode1_radio = tui_create_radiobutton("mode1_radio", radio1_bounds, "Режим 1", "mode_group");
    if (app->mode1_radio) {
        app->mode1_radio->checked = true;
        app->mode1_radio->change_handler = tui_demo_mode_radio_handler;
        app->mode1_radio->base.user_data = app;
        tui_group_add_widget(app->control_group, (tui_widget_t*)app->mode1_radio);
    }

    tui_rect_t radio2_bounds = {.pos = {2, 10}, .size = {20, 1}};
    app->mode2_radio = tui_create_radiobutton("mode2_radio", radio2_bounds, "Режим 2", "mode_group");
    if (app->mode2_radio) {
        app->mode2_radio->change_handler = tui_demo_mode_radio_handler;
        app->mode2_radio->base.user_data = app;
        tui_group_add_widget(app->control_group, (tui_widget_t*)app->mode2_radio);
    }

    tui_rect_t radio3_bounds = {.pos = {2, 11}, .size = {20, 1}};
    app->mode3_radio = tui_create_radiobutton("mode3_radio", radio3_bounds, "Режим 3", "mode_group");
    if (app->mode3_radio) {
        app->mode3_radio->change_handler = tui_demo_mode_radio_handler;
        app->mode3_radio->base.user_data = app;
        tui_group_add_widget(app->control_group, (tui_widget_t*)app->mode3_radio);
    }
}

// Создание панели отображения
void tui_demo_create_display_panel(tui_demo_app_t* app) {
    if (!app) return;
    
    tui_rect_t display_bounds = {
        .pos = {28, 5},
        .size = {50, 15}
    };
    
    app->display_group = tui_create_group("display_group", display_bounds, "Отображение");
    if (!app->display_group) return;
    
    // Прогресс-бар
    tui_rect_t progress_bounds = {.pos = {30, 7}, .size = {46, 3}};
    app->progress_bar = tui_create_progressbar("progress_bar", progress_bounds, "Прогресс выполнения");
    if (app->progress_bar) {
        app->progress_bar->show_percentage = true;
        tui_group_add_widget(app->display_group, (tui_widget_t*)app->progress_bar);
    }
    
    // Список логов
    tui_rect_t log_bounds = {.pos = {30, 11}, .size = {46, 7}};
    app->log_list = tui_create_list("log_list", log_bounds);
    if (app->log_list) {
        // Добавляем несколько тестовых сообщений
        tui_list_add_item(app->log_list, "Демонстрация TUI запущена", NULL);
        tui_list_add_item(app->log_list, "Система готова к работе", NULL);
        tui_list_add_item(app->log_list, "Нажмите 'Старт' для начала", NULL);
        tui_group_add_widget(app->display_group, (tui_widget_t*)app->log_list);
    }
    
    // Поле ввода
    tui_rect_t input_bounds = {.pos = {30, 19}, .size = {46, 1}};
    app->input_box = tui_create_textbox("input_box", input_bounds, "Введите команду...");
    if (app->input_box) {
        app->input_box->change_handler = tui_demo_input_change_handler;
        app->input_box->base.user_data = app;
        tui_group_add_widget(app->display_group, (tui_widget_t*)app->input_box);
    }
}

// Создание статус-бара
void tui_demo_create_statusbar(tui_demo_app_t* app) {
    if (!app) return;
    
    tui_rect_t status_bounds = {
        .pos = {1, 21},
        .size = {78, 3}
    };
    
    app->statusbar = tui_create_statusbar("main_statusbar", status_bounds);
    if (!app->statusbar) return;
    
    // Устанавливаем начальный текст
    tui_statusbar_set_left_text(app->statusbar, "MyOS TUI Demo v1.0");
    tui_statusbar_set_center_text(app->statusbar, "Готов к работе");
    tui_statusbar_set_right_text(app->statusbar, "Ctrl+Q для выхода");
}

// Обработка клавиатуры
void tui_demo_handle_keyboard(tui_demo_app_t* app, tui_event_t* event) {
    if (!app || !event) return;
    
    if (event->type == TUI_EVENT_KEY_PRESS) {
        if (event->data.key.ctrl && event->data.key.key_code == 'Q') {
            // Ctrl+Q - выход из демонстрации
            app->running = false;
        }
    }
}

// Обработка мыши
void tui_demo_handle_mouse(tui_demo_app_t* app, tui_event_t* event) {
    if (!app || !event) return;

    if (event->type == TUI_EVENT_MOUSE_CLICK) {
        // Реализуем обработку кликов мыши
        uint16_t click_x = event->data.mouse.pos.x;
        uint16_t click_y = event->data.mouse.pos.y;

        // Проверяем, нажата ли кнопка Старт
        if (app->start_button && app->start_button->base.visible) {
            tui_rect_t btn_bounds = app->start_button->base.bounds;
            if (click_x >= btn_bounds.pos.x && click_x < btn_bounds.pos.x + btn_bounds.size.width &&
                click_y >= btn_bounds.pos.y && click_y < btn_bounds.pos.y + btn_bounds.size.height) {
                if (app->start_button->click_handler) {
                    app->start_button->click_handler(app);
                }
            }
        }

        // Проверяем, нажата ли кнопка Стоп
        if (app->stop_button && app->stop_button->base.visible) {
            tui_rect_t btn_bounds = app->stop_button->base.bounds;
            if (click_x >= btn_bounds.pos.x && click_x < btn_bounds.pos.x + btn_bounds.size.width &&
                click_y >= btn_bounds.pos.y && click_y < btn_bounds.pos.y + btn_bounds.size.height) {
                if (app->stop_button->click_handler) {
                    app->stop_button->click_handler(app);
                }
            }
        }

        // Проверяем, нажата ли кнопка Сброс
        if (app->reset_button && app->reset_button->base.visible) {
            tui_rect_t btn_bounds = app->reset_button->base.bounds;
            if (click_x >= btn_bounds.pos.x && click_x < btn_bounds.pos.x + btn_bounds.size.width &&
                click_y >= btn_bounds.pos.y && click_y < btn_bounds.pos.y + btn_bounds.size.height) {
                if (app->reset_button->click_handler) {
                    app->reset_button->click_handler(app);
                }
            }
        }
    }
}

// Центрирование окна
void tui_demo_center_window(tui_window_t* window) {
    if (!window) return;
    
    uint16_t screen_width = tui_get_screen_width();
    uint16_t screen_height = tui_get_screen_height();
    
    uint16_t center_x = (screen_width - window->base.bounds.size.width) / 2;
    uint16_t center_y = (screen_height - window->base.bounds.size.height) / 2;
    
    tui_pos_t new_pos = {center_x, center_y};
    tui_move_window(window, new_pos);
}

// Расчет макета
tui_rect_t tui_demo_calculate_layout(tui_size_t screen_size) {
    tui_rect_t layout = {
        .pos = {1, 1},
        .size = {screen_size.width - 2, screen_size.height - 2}
    };
    return layout;
}

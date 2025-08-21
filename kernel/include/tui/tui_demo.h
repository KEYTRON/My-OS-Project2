#ifndef TUI_DEMO_H
#define TUI_DEMO_H

#include "tui.h"
#include "widgets.h"

// Демонстрационное приложение
typedef struct {
    tui_window_t* main_window;
    tui_toolbar_t* toolbar;
    tui_statusbar_t* statusbar;
    tui_group_t* control_group;
    tui_group_t* display_group;
    
    // Виджеты управления
    tui_button_t* start_button;
    tui_button_t* stop_button;
    tui_button_t* reset_button;
    tui_checkbox_t* auto_checkbox;
    tui_radiobutton_t* mode1_radio;
    tui_radiobutton_t* mode2_radio;
    tui_radiobutton_t* mode3_radio;
    
    // Виджеты отображения
    tui_progressbar_t* progress_bar;
    tui_list_t* log_list;
    tui_textbox_t* input_box;
    
    // Состояние
    bool running;
    uint8_t progress;
    uint32_t start_time;
    uint32_t last_update;
} tui_demo_app_t;

// Функции демонстрации
tui_demo_app_t* tui_demo_create(void);
void tui_demo_destroy(tui_demo_app_t* app);
void tui_demo_run(tui_demo_app_t* app);
void tui_demo_cleanup(tui_demo_app_t* app);

// Обработчики событий
void tui_demo_start_button_handler(void* data);
void tui_demo_stop_button_handler(void* data);
void tui_demo_reset_button_handler(void* data);
void tui_demo_auto_checkbox_handler(void* data);
void tui_demo_mode_radio_handler(void* data);
void tui_demo_input_change_handler(void* data);

// Обновление демонстрации
void tui_demo_update(tui_demo_app_t* app);
void tui_demo_update_progress(tui_demo_app_t* app);
void tui_demo_add_log_message(tui_demo_app_t* app, const char* message);

// Создание интерфейса
void tui_demo_create_interface(tui_demo_app_t* app);
void tui_demo_create_toolbar(tui_demo_app_t* app);
void tui_demo_create_control_panel(tui_demo_app_t* app);
void tui_demo_create_display_panel(tui_demo_app_t* app);
void tui_demo_create_statusbar(tui_demo_app_t* app);

// Обработка ввода
void tui_demo_handle_keyboard(tui_demo_app_t* app, tui_event_t* event);
void tui_demo_handle_mouse(tui_demo_app_t* app, tui_event_t* event);

// Утилиты
void tui_demo_center_window(tui_window_t* window);
tui_rect_t tui_demo_calculate_layout(tui_size_t screen_size);

#endif // TUI_DEMO_H

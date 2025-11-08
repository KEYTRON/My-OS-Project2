#include "../../include/tui/tui.h"
#include "../drivers/vga.h"
#include "../drivers/keyboard.h"
#include "../lib/string.h"
#include "../lib/printf.h"

// Глобальная TUI система
static tui_system_t g_tui_system = {0};

// Буфер клавиатуры для TUI
#define KEYBOARD_BUFFER_SIZE 256
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE] = {0};
static uint16_t keyboard_head = 0;
static uint16_t keyboard_tail = 0;
static bool tui_should_exit = false;

// Инициализация TUI системы
bool tui_system_init(void) {
    // Получаем размер экрана
    g_tui_system.screen_size.width = 80;  // VGA текстовая ширина
    g_tui_system.screen_size.height = 25; // VGA текстовая высота
    
    // Выделяем память для буферов
    size_t buffer_size = g_tui_system.screen_size.width * g_tui_system.screen_size.height * sizeof(tui_char_t);
    
    g_tui_system.screen_buffer = (tui_char_t*)0x1000000; // Временный адрес
    g_tui_system.back_buffer = (tui_char_t*)0x1000000 + buffer_size;
    
    if (!g_tui_system.screen_buffer || !g_tui_system.back_buffer) {
        return false;
    }
    
    // Инициализируем буферы
    tui_char_t default_char = {
        .character = ' ',
        .fg_color = TUI_COLOR_WHITE,
        .bg_color = TUI_COLOR_BLACK,
        .attributes = TUI_ATTR_NORMAL
    };
    
    for (int i = 0; i < g_tui_system.screen_size.width * g_tui_system.screen_size.height; i++) {
        g_tui_system.screen_buffer[i] = default_char;
        g_tui_system.back_buffer[i] = default_char;
    }
    
    // Инициализируем остальные поля
    g_tui_system.windows = NULL;
    g_tui_system.menus = NULL;
    g_tui_system.focused_window = NULL;
    g_tui_system.cursor_pos.x = 0;
    g_tui_system.cursor_pos.y = 0;
    g_tui_system.cursor_visible = true;
    g_tui_system.default_fg = TUI_COLOR_WHITE;
    g_tui_system.default_bg = TUI_COLOR_BLACK;
    g_tui_system.tick_count = 0;
    
    // Очищаем экран
    tui_clear_screen();
    
    return true;
}

// Очистка TUI системы
void tui_system_cleanup(void) {
    // Уничтожаем все окна
    tui_window_t* window = g_tui_system.windows;
    while (window) {
        tui_window_t* next = (tui_window_t*)window->base.next;
        tui_destroy_window(window);
        window = next;
    }
    g_tui_system.windows = NULL;

    // Уничтожаем все меню
    tui_menu_t* menu = g_tui_system.menus;
    while (menu) {
        tui_menu_t* next = (tui_menu_t*)menu->base.next;
        tui_destroy_menu(menu);
        menu = next;
    }
    g_tui_system.menus = NULL;

    // Очищаем буферы
    g_tui_system.screen_buffer = NULL;
    g_tui_system.back_buffer = NULL;
}

// Обновление TUI системы
void tui_system_update(void) {
    g_tui_system.tick_count++;
    
    // Обновляем все окна
    tui_window_t* window = g_tui_system.windows;
    if (window && window->base.visible) {
        tui_draw_window(window);
    }
    
    // Обновляем все меню
    tui_menu_t* menu = g_tui_system.menus;
    if (menu && menu->base.visible) {
        tui_draw_menu(menu);
    }
    
    // Обновляем курсор
    if (g_tui_system.cursor_visible) {
        tui_set_cursor_pos(g_tui_system.cursor_pos);
    }
}

// Обработка событий
void tui_system_handle_event(tui_event_t* event) {
    if (!event) return;

    // Обрабатываем системные события
    switch (event->type) {
        case TUI_EVENT_KEY_PRESS:
            // Обработка системных клавиш
            if (event->data.key.ctrl && event->data.key.key_code == 'q') {
                // Ctrl+Q - выход из TUI
                tui_should_exit = true;
                return;
            }
            // Добавляем символ в буфер
            if (event->data.key.key_code > 0) {
                uint16_t next_head = (keyboard_head + 1) % KEYBOARD_BUFFER_SIZE;
                if (next_head != keyboard_tail) {
                    keyboard_buffer[keyboard_head] = event->data.key.key_code;
                    keyboard_head = next_head;
                }
            }
            break;

        case TUI_EVENT_MOUSE_CLICK:
            // Обработка кликов мыши
            break;

        case TUI_EVENT_WINDOW_RESIZE:
            // Обработка изменения размера экрана
            break;

        default:
            break;
    }

    // Передаем событие в сфокусированное окно
    if (g_tui_system.focused_window && g_tui_system.focused_window->base.event_handler) {
        g_tui_system.focused_window->base.event_handler(event);
    }
}

// Очистка экрана
void tui_clear_screen(void) {
    tui_char_t clear_char = {
        .character = ' ',
        .fg_color = g_tui_system.default_fg,
        .bg_color = g_tui_system.default_bg,
        .attributes = TUI_ATTR_NORMAL
    };

    for (int i = 0; i < g_tui_system.screen_size.width * g_tui_system.screen_size.height; i++) {
        g_tui_system.screen_buffer[i] = clear_char;
        g_tui_system.back_buffer[i] = clear_char;
    }

    // Обновляем VGA буфер
    vga_init();
}

// Установка позиции курсора
void tui_set_cursor_pos(tui_pos_t pos) {
    if (pos.x >= g_tui_system.screen_size.width) pos.x = g_tui_system.screen_size.width - 1;
    if (pos.y >= g_tui_system.screen_size.height) pos.y = g_tui_system.screen_size.height - 1;

    g_tui_system.cursor_pos = pos;

    // Обновляем VGA курсор через port I/O (VGA CRTC) - только на x86_64
#ifdef __x86_64__
    uint16_t offset = pos.y * g_tui_system.screen_size.width + pos.x;
    // Устанавливаем высокий байт смещения курсора
    __asm__ __volatile__("outb %%al, $0x3D4" : : "a"(0x0E));
    __asm__ __volatile__("outb %%al, $0x3D5" : : "a"((offset >> 8) & 0xFF));
    // Устанавливаем низкий байт смещения курсора
    __asm__ __volatile__("outb %%al, $0x3D4" : : "a"(0x0F));
    __asm__ __volatile__("outb %%al, $0x3D5" : : "a"(offset & 0xFF));
#endif
}

// Показать/скрыть курсор
void tui_show_cursor(bool show) {
    g_tui_system.cursor_visible = show;
    if (show) {
        tui_set_cursor_pos(g_tui_system.cursor_pos);
    } else {
        // Скрываем курсор (ставим в невидимую позицию - за границу экрана)
#ifdef __x86_64__
        uint16_t offset = g_tui_system.screen_size.width * g_tui_system.screen_size.height;
        __asm__ __volatile__("outb %%al, $0x3D4" : : "a"(0x0E));
        __asm__ __volatile__("outb %%al, $0x3D5" : : "a"((offset >> 8) & 0xFF));
        __asm__ __volatile__("outb %%al, $0x3D4" : : "a"(0x0F));
        __asm__ __volatile__("outb %%al, $0x3D5" : : "a"(offset & 0xFF));
#endif
    }
}

// Установка цветов по умолчанию
void tui_set_default_colors(tui_color_t fg, tui_color_t bg) {
    g_tui_system.default_fg = fg;
    g_tui_system.default_bg = bg;
}

// Рисование символа
void tui_draw_char(tui_pos_t pos, char c, tui_color_t fg, tui_color_t bg) {
    if (pos.x >= g_tui_system.screen_size.width || pos.y >= g_tui_system.screen_size.height) {
        return;
    }

    size_t index = pos.y * g_tui_system.screen_size.width + pos.x;
    g_tui_system.back_buffer[index].character = c;
    g_tui_system.back_buffer[index].fg_color = fg;
    g_tui_system.back_buffer[index].bg_color = bg;
    g_tui_system.back_buffer[index].attributes = TUI_ATTR_NORMAL;

    // Обновляем VGA буфер напрямую (0xB8000 - текстовый режим VGA)
    uint16_t* vga_buffer = (uint16_t*)0xB8000;
    uint8_t vga_color = vga_entry_color(fg, bg); // Функция из vga.h
    vga_buffer[index] = (uint16_t)c | ((uint16_t)vga_color << 8);
}

// Рисование строки
void tui_draw_string(tui_pos_t pos, const char* str, tui_color_t fg, tui_color_t bg) {
    if (!str) return;
    
    tui_pos_t current_pos = pos;
    while (*str && current_pos.x < g_tui_system.screen_size.width) {
        tui_draw_char(current_pos, *str, fg, bg);
        current_pos.x++;
        str++;
    }
}

// Рисование строки в указанной позиции (алиас для совместимости)
void tui_draw_string_at(tui_pos_t pos, const char* str, tui_color_t fg, tui_color_t bg) {
    tui_draw_string(pos, str, fg, bg);
}

// Рисование прямоугольника
void tui_draw_rect(tui_rect_t rect, char fill_char, tui_color_t fg, tui_color_t bg) {
    for (uint16_t y = rect.pos.y; y < rect.pos.y + rect.size.height && y < g_tui_system.screen_size.height; y++) {
        for (uint16_t x = rect.pos.x; x < rect.pos.x + rect.size.width && x < g_tui_system.screen_size.width; x++) {
            tui_pos_t pos = {x, y};
            tui_draw_char(pos, fill_char, fg, bg);
        }
    }
}

// Рисование границы
void tui_draw_border(tui_rect_t rect, tui_color_t color) {
    // Верхняя и нижняя границы
    for (uint16_t x = rect.pos.x; x < rect.pos.x + rect.size.width; x++) {
        tui_pos_t top_pos = {x, rect.pos.y};
        tui_pos_t bottom_pos = {x, rect.pos.y + rect.size.height - 1};
        tui_draw_char(top_pos, '-', color, g_tui_system.default_bg);
        tui_draw_char(bottom_pos, '-', color, g_tui_system.default_bg);
    }
    
    // Левая и правая границы
    for (uint16_t y = rect.pos.y; y < rect.pos.y + rect.size.height; y++) {
        tui_pos_t left_pos = {rect.pos.x, y};
        tui_pos_t right_pos = {rect.pos.x + rect.size.width - 1, y};
        tui_draw_char(left_pos, '|', color, g_tui_system.default_bg);
        tui_draw_char(right_pos, '|', color, g_tui_system.default_bg);
    }
    
    // Углы
    tui_draw_char(rect.pos, '+', color, g_tui_system.default_bg);
    tui_pos_t top_right = {rect.pos.x + rect.size.width - 1, rect.pos.y};
    tui_pos_t bottom_left = {rect.pos.x, rect.pos.y + rect.size.height - 1};
    tui_pos_t bottom_right = {rect.pos.x + rect.size.width - 1, rect.pos.y + rect.size.height - 1};
    
    tui_draw_char(top_right, '+', color, g_tui_system.default_bg);
    tui_draw_char(bottom_left, '+', color, g_tui_system.default_bg);
    tui_draw_char(bottom_right, '+', color, g_tui_system.default_bg);
}

// Заполнение прямоугольника
void tui_fill_rect(tui_rect_t rect, char fill_char, tui_color_t fg, tui_color_t bg) {
    tui_draw_rect(rect, fill_char, fg, bg);
}

// Утилиты
tui_color_t tui_make_color(uint8_t r, uint8_t g, uint8_t b) {
    // Простое преобразование RGB в VGA цвета
    if (r > 128 && g > 128 && b > 128) return TUI_COLOR_WHITE;
    if (r > 128) return TUI_COLOR_RED;
    if (g > 128) return TUI_COLOR_GREEN;
    if (b > 128) return TUI_COLOR_BLUE;
    return TUI_COLOR_BLACK;
}

uint16_t tui_get_screen_width(void) {
    return g_tui_system.screen_size.width;
}

uint16_t tui_get_screen_height(void) {
    return g_tui_system.screen_size.height;
}

tui_pos_t tui_get_cursor_pos(void) {
    return g_tui_system.cursor_pos;
}

uint32_t tui_get_tick_count(void) {
    return g_tui_system.tick_count;
}

// Функция для добавления символа в буфер (вызывается из обработчика клавиатуры)
void tui_add_char_to_buffer(char c) {
    uint16_t next_head = (keyboard_head + 1) % KEYBOARD_BUFFER_SIZE;
    if (next_head != keyboard_tail) {
        keyboard_buffer[keyboard_head] = c;
        keyboard_head = next_head;
    }
}

// Получение флага выхода
bool tui_should_exit_loop(void) {
    return tui_should_exit;
}

// Сброс флага выхода
void tui_reset_exit_flag(void) {
    tui_should_exit = false;
}

// Обработка ввода
bool tui_is_key_pressed(uint16_t key_code) {
    (void)key_code;  // Unused parameter
    // Проверяем, есть ли символ в буфере
    return keyboard_head != keyboard_tail;
}

char tui_get_char(void) {
    // Получаем символ из буфера
    if (keyboard_head == keyboard_tail) {
        return '\0';
    }
    char c = keyboard_buffer[keyboard_tail];
    keyboard_tail = (keyboard_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

tui_event_t* tui_get_event(void) {
    // Получение события из очереди
    // В настоящее время события обрабатываются напрямую
    return NULL;
}

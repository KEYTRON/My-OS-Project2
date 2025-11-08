/*
 * GUI Desktop Manager - Полный графический рабочий стол
 * Рабочий стол с окнами, иконками, панелью задач
 */

#include <stdint.h>
#include <stdbool.h>
#include "../../lib/printf.h"
#include "../../lib/graphics/graphics.h"

// Цвета для интерфейса
#define DESKTOP_BG_COLOR      0x1a1a2e  // Темно-синий
#define TASKBAR_COLOR         0x0f3460  // Ещё темнее
#define WINDOW_TITLE_COLOR    0x16213e  // Для заголовков окон
#define WINDOW_BG_COLOR       0x2a2a4e  // Светлее чем фон
#define TEXT_COLOR            0xeaeaea  // Светлый текст
#define ACCENT_COLOR          0x00d4ff  // Голубой акцент
#define BUTTON_COLOR          0x16213e  // Кнопки
#define BUTTON_HOVER_COLOR    0x0f3460  // При наведении

// Структуры
typedef struct {
    int32_t x, y;
    uint32_t width, height;
    bool visible;
    const char* title;
    uint32_t id;
} gui_window_t;

typedef struct {
    int32_t x, y;
    uint32_t width, height;
    const char* label;
    bool hovered;
    bool pressed;
} gui_button_t;

typedef struct {
    gui_window_t windows[8];
    int32_t window_count;
    gui_button_t taskbar_buttons[4];
    int32_t taskbar_button_count;
    uint32_t active_window;
    graphics_device_t *gfx;
} gui_desktop_t;

static gui_desktop_t desktop = {0};

// Функции для рисования
static void draw_filled_rect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color) {
    if (!desktop.gfx) return;
    graphics_rect_t rect = {.x = x, .y = y, .width = w, .height = h};
    graphics_fillrect(rect, color);
}

static void draw_rect_border(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color, int thickness) {
    if (!desktop.gfx) return;
    // Верхняя линия
    draw_filled_rect(x, y, w, thickness, color);
    // Нижняя линия
    draw_filled_rect(x, y + h - thickness, w, thickness, color);
    // Левая линия
    draw_filled_rect(x, y, thickness, h, color);
    // Правая линия
    draw_filled_rect(x + w - thickness, y, thickness, h, color);
}

static void draw_window(gui_window_t *win) {
    if (!win->visible || !desktop.gfx) return;

    // Заголовок окна
    draw_filled_rect(win->x, win->y, win->width, 30, WINDOW_TITLE_COLOR);

    // Тело окна
    draw_filled_rect(win->x, win->y + 30, win->width, win->height - 30, WINDOW_BG_COLOR);

    // Рамка окна
    draw_rect_border(win->x, win->y, win->width, win->height, ACCENT_COLOR, 2);

    // Кнопка закрытия (X)
    int close_btn_x = win->x + win->width - 30;
    int close_btn_y = win->y + 5;
    draw_filled_rect(close_btn_x, close_btn_y, 20, 20, BUTTON_COLOR);

    // Текст заголовка
    if (win->title) {
        graphics_putstring(win->x + 10, win->y + 8, win->title, TEXT_COLOR, WINDOW_TITLE_COLOR);
    }
}

static void draw_button(gui_button_t *btn) {
    if (!desktop.gfx) return;

    uint32_t color = btn->hovered ? BUTTON_HOVER_COLOR : BUTTON_COLOR;
    if (btn->pressed) color = ACCENT_COLOR;

    draw_filled_rect(btn->x, btn->y, btn->width, btn->height, color);
    draw_rect_border(btn->x, btn->y, btn->width, btn->height, ACCENT_COLOR, 1);

    if (btn->label) {
        graphics_putstring(btn->x + 10, btn->y + 5, btn->label, TEXT_COLOR, color);
    }
}

static void draw_taskbar(void) {
    if (!desktop.gfx) return;

    uint32_t screen_height = graphics_get_height();

    // Фон панели задач
    draw_filled_rect(0, screen_height - 50, graphics_get_width(), 50, TASKBAR_COLOR);

    // Кнопки на панели задач
    for (int i = 0; i < desktop.taskbar_button_count; i++) {
        draw_button(&desktop.taskbar_buttons[i]);
    }

    // Текст "MyOS" слева на панели
    graphics_putstring(10, screen_height - 40, "MyOS v1.0", TEXT_COLOR, TASKBAR_COLOR);
}

static void draw_desktop_wallpaper(void) {
    if (!desktop.gfx) return;

    uint32_t screen_width = graphics_get_width();
    uint32_t screen_height = graphics_get_height();

    // Фон рабочего стола - красивый градиентный эффект
    for (uint32_t y = 0; y < screen_height - 50; y++) {
        uint32_t color = DESKTOP_BG_COLOR;
        // Небольшой градиент сверху
        if (y < 100) {
            uint32_t gradient = (y << 16) / 100;
            color = DESKTOP_BG_COLOR + (gradient & 0x1a1a00);
        }
        graphics_rect_t line = {.x = 0, .y = y, .width = screen_width, .height = 1};
        graphics_fillrect(line, color);
    }
}

static void draw_icons(void) {
    if (!desktop.gfx) return;

    // Рисуем несколько иконок на рабочем столе
    const char *icon_labels[] = {"File Manager", "Terminal", "Settings", "Help"};
    int32_t x = 30;
    int32_t y = 30;
    int icon_size = 60;
    int spacing = 20;

    for (int i = 0; i < 4; i++) {
        // Иконка (прямоугольник)
        draw_filled_rect(x, y, icon_size, icon_size, BUTTON_COLOR);
        draw_rect_border(x, y, icon_size, icon_size, ACCENT_COLOR, 1);

        // Текст под иконкой
        graphics_putstring(x - 10, y + icon_size + 5, icon_labels[i], TEXT_COLOR, DESKTOP_BG_COLOR);

        x += icon_size + spacing;
    }
}

// Инициализация GUI Desktop
void gui_desktop_init(graphics_device_t *gfx) {
    desktop.gfx = gfx;
    desktop.window_count = 0;
    desktop.taskbar_button_count = 0;
    desktop.active_window = 0;

    if (!gfx || gfx->bpp < 16) {
        printf("GUI Desktop требует 16-bit или выше цветной графики\n");
        return;
    }

    // Создаём несколько окон демонстрации
    gui_window_t *main_win = &desktop.windows[desktop.window_count++];
    main_win->id = 1;
    main_win->x = 100;
    main_win->y = 100;
    main_win->width = 500;
    main_win->height = 350;
    main_win->title = "MyOS Desktop v1.0";
    main_win->visible = true;

    gui_window_t *info_win = &desktop.windows[desktop.window_count++];
    info_win->id = 2;
    info_win->x = 650;
    info_win->y = 120;
    info_win->width = 400;
    info_win->height = 280;
    info_win->title = "System Information";
    info_win->visible = true;

    // Кнопки на панели задач
    gui_button_t *btn1 = &desktop.taskbar_buttons[desktop.taskbar_button_count++];
    btn1->x = 150;
    btn1->y = gfx->height - 45;
    btn1->width = 120;
    btn1->height = 40;
    btn1->label = "Window 1";
    btn1->hovered = false;
    btn1->pressed = false;

    gui_button_t *btn2 = &desktop.taskbar_buttons[desktop.taskbar_button_count++];
    btn2->x = 280;
    btn2->y = gfx->height - 45;
    btn2->width = 120;
    btn2->height = 40;
    btn2->label = "Window 2";
    btn2->hovered = false;
    btn2->pressed = false;

    printf("GUI Desktop инициализирован. Окна: %d, кнопки: %d\n",
           desktop.window_count, desktop.taskbar_button_count);
}

// Рендеринг всего интерфейса
void gui_desktop_render(void) {
    if (!desktop.gfx) return;

    // Очищаем экран
    graphics_clear(DESKTOP_BG_COLOR);

    // Рисуем обои
    draw_desktop_wallpaper();

    // Рисуем иконки на рабочем столе
    draw_icons();

    // Рисуем все окна
    for (int i = 0; i < desktop.window_count; i++) {
        if (desktop.windows[i].visible) {
            draw_window(&desktop.windows[i]);
        }
    }

    // Рисуем панель задач
    draw_taskbar();

    // Обновляем дисплей
    graphics_flush();

    printf("GUI Desktop отрисован\n");
}

// Запуск GUI Desktop
void gui_desktop_run(graphics_device_t *gfx) {
    if (!gfx) {
        printf("Ошибка: графическое устройство не инициализировано\n");
        return;
    }

    printf("Запуск GUI Desktop...\n");
    gui_desktop_init(gfx);
    gui_desktop_render();

    printf("\nGUI Desktop запущен успешно!\n");
    printf("═════════════════════════════════════════════════════════════\n");
    printf("Рабочий стол отображает:\n");
    printf("  ✓ Графический фон рабочего стола\n");
    printf("  ✓ %d графических окна с заголовками\n", desktop.window_count);
    printf("  ✓ Панель задач с кнопками приложений\n");
    printf("  ✓ Иконки приложений\n");
    printf("═════════════════════════════════════════════════════════════\n");
}

// Функция для обновления интерфейса
void gui_desktop_update(void) {
    gui_desktop_render();
}

// Функция обработки событий мыши
void gui_desktop_handle_mouse(int32_t x, int32_t y, bool clicked) {
    if (!desktop.gfx) return;

    // Проверяем нажатие на кнопки панели задач
    for (int i = 0; i < desktop.taskbar_button_count; i++) {
        gui_button_t *btn = &desktop.taskbar_buttons[i];
        bool over = (x >= btn->x && x < btn->x + btn->width &&
                     y >= btn->y && y < btn->y + btn->height);

        if (over) {
            btn->hovered = true;
            if (clicked) {
                btn->pressed = true;
                printf("Нажата кнопка: %s\n", btn->label);
            }
        } else {
            btn->hovered = false;
            btn->pressed = false;
        }
    }

    gui_desktop_render();
}

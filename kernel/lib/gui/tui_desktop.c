/*
 * ASCII Terminal User Interface (TUI) Desktop
 * Реальный рабочий стол в консоли - вы сможете его увидеть!
 */

#include <stdint.h>
#include <stdbool.h>
#include "../../lib/printf.h"

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 25

typedef struct {
    int x, y;
    int width, height;
    const char* title;
    const char* content;
    bool visible;
} tui_window_t;

static tui_window_t windows[4];
static int window_count = 0;

static void tui_clear_screen(void) {
    printf("\033[2J");  // ANSI clear screen
    printf("\033[H");   // ANSI home cursor
}

static void tui_set_cursor(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);  // ANSI set cursor
}

static void tui_draw_box(int x, int y, int w, int h, const char* title) {
    tui_set_cursor(x, y);
    printf("┌");
    for (int i = 1; i < w - 1; i++) printf("─");
    printf("┐");

    for (int row = 1; row < h - 1; row++) {
        tui_set_cursor(x, y + row);
        printf("│");
        tui_set_cursor(x + w - 1, y + row);
        printf("│");
    }

    tui_set_cursor(x, y + h - 1);
    printf("└");
    for (int i = 1; i < w - 1; i++) printf("─");
    printf("┘");

    if (title) {
        tui_set_cursor(x + 2, y);
        printf(" %s ", title);
    }
}

static void tui_draw_text(int x, int y, const char* text) {
    tui_set_cursor(x, y);
    printf("%s", text);
}

static void tui_window_create(const char* title, int x, int y, int w, int h, const char* content) {
    if (window_count >= 4) return;

    tui_window_t* win = &windows[window_count++];
    win->x = x;
    win->y = y;
    win->width = w;
    win->height = h;
    win->title = title;
    win->content = content;
    win->visible = true;
}

static void tui_desktop_render(void) {
    tui_clear_screen();

    // Заголовок
    printf("\033[1;34m");  // Blue + Bold
    printf("╔════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                     MyOS v1.0 - Desktop Environment                            ║\n");
    printf("║                      Multi-Architecture GUI Kernel                             ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\033[0m");  // Reset

    // Рисуем окна
    for (int i = 0; i < window_count; i++) {
        tui_window_t* win = &windows[i];
        if (!win->visible) continue;

        tui_draw_box(win->x, win->y + 3, win->width, win->height, win->title);

        if (win->content) {
            tui_set_cursor(win->x + 2, win->y + 5);
            printf("%s", win->content);
        }
    }

    // Информация об окнах
    printf("\033[1;32m");  // Green + Bold
    tui_set_cursor(2, 22);
    printf("┌─ Открытые окна ─────────────────────┐");
    tui_set_cursor(2, 23);
    printf("│ Window 1: MyOS Desktop (50x15)      │");
    tui_set_cursor(2, 24);
    printf("└─────────────────────────────────────┘");
    printf("\033[0m");  // Reset

    // Статусная строка
    printf("\033[1;33m");  // Yellow + Bold
    tui_set_cursor(45, 22);
    printf("┌─ System Info ──────────┐");
    tui_set_cursor(45, 23);
    printf("│ RAM: 512 MB             │");
    tui_set_cursor(45, 24);
    printf("└─────────────────────────┘");
    printf("\033[0m");  // Reset
}

static void tui_desktop_init(void) {
    window_count = 0;

    // Создаем окна
    tui_window_create(
        "MyOS Desktop",
        2, 2, 50, 15,
        "Welcome to MyOS v1.0\n"
        "┌──────────────────────┐\n"
        "│ [  Click Me!  ]      │\n"
        "│  Welcome to GUI!     │\n"
        "└──────────────────────┘"
    );

    tui_window_create(
        "System Info",
        54, 5, 24, 12,
        "Architecture: x86_64\n"
        "RAM: 512 MB\n"
        "Display: ASCII TUI\n"
        "Status: Running ✓"
    );
}

void tui_desktop_run(void) {
    tui_desktop_init();
    tui_desktop_render();

    printf("\033[1;36m");  // Cyan + Bold
    printf("\n");
    tui_set_cursor(0, 23);
    printf("════════════════════════════════════════════════════════════════════════════════════");
    tui_set_cursor(0, 24);
    printf("Ready. Press any key to continue...");
    printf("\033[0m\n");
}

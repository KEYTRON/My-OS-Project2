/*
 * ASCII Terminal User Interface (TUI) Desktop
 * Рабочий стол, нарисованный в консоли символами
 */

#ifndef TUI_DESKTOP_H
#define TUI_DESKTOP_H

/**
 * Инициализирует TUI рабочий стол
 */
void tui_desktop_init(void);

/**
 * Рисует рабочий стол в консоль
 */
void tui_desktop_render(void);

/**
 * Главная функция запуска рабочего стола
 */
void tui_desktop_run(void);

/**
 * Очищает экран
 */
void tui_clear_screen(void);

/**
 * Устанавливает позицию курсора
 */
void tui_set_cursor(int x, int y);

#endif /* TUI_DESKTOP_H */

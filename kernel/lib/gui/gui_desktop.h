/*
 * GUI Desktop Manager - Полный графический рабочий стол
 */

#ifndef GUI_DESKTOP_H
#define GUI_DESKTOP_H

#include "../../lib/graphics/graphics.h"

/**
 * Инициализирует GUI Desktop
 * @param gfx Указатель на графическое устройство
 */
void gui_desktop_init(graphics_device_t *gfx);

/**
 * Рендерит весь интерфейс рабочего стола
 */
void gui_desktop_render(void);

/**
 * Запускает GUI Desktop полностью
 * @param gfx Указатель на графическое устройство
 */
void gui_desktop_run(graphics_device_t *gfx);

/**
 * Обновляет интерфейс рабочего стола
 */
void gui_desktop_update(void);

/**
 * Обрабатывает события мыши
 * @param x X координата мыши
 * @param y Y координата мыши
 * @param clicked Нажата ли левая кнопка мыши
 */
void gui_desktop_handle_mouse(int32_t x, int32_t y, bool clicked);

#endif /* GUI_DESKTOP_H */

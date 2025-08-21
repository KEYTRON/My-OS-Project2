// keyboard.h — обработчик PS/2 клавиатуры
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

// Таблица соответствия scan code → ASCII (для простейших символов)
extern char keymap[128];

// Функция инициализации клавиатуры (в основном регистрируем обработчик IRQ1)
void keyboard_init();

// Функция-обработчик (вызывается из irq_handler)
void keyboard_callback();

#endif // KEYBOARD_H

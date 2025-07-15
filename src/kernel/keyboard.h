#ifndef KEYBOARD_H
#define KEYBOARD_H

// Инициализация клавиатуры
void init_keyboard();

// Получить следующий символ (блокируется до нажатия)
char keyboard_get_char();

// Считать строку (до ввода Enter)
void read_line(char *buf, int max_len);

#endif

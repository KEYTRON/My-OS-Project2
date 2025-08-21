// serial.h — работа с COM-портом (UART)
#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

// Инициализация COM1 порта
void serial_init();

// Отправка одного символа
void serial_write_char(char c);

// Отправка строки
void serial_write_string(const char* str);

#endif // SERIAL_H
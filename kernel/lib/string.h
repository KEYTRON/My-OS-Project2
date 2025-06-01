// string.h — базовые функции работы со строками
#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stddef.h>

// Возвращает длину С‑строки (без нулевого символа)
size_t strlen(const char* str);

// Копирует строку (включая нулевой символ)
char* strcpy(char* dest, const char* src);

// Сравнивает две строки (аналог strcmp)
int strcmp(const char* a, const char* b);

#endif // STRING_H
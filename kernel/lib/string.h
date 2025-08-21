// string.h — базовые функции работы со строками
#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stddef.h>

// Возвращает длину С-строки (без нулевого символа)
size_t strlen(const char* str);

// Копирует строку (включая нулевой символ)
char* strcpy(char* dest, const char* src);

// Сравнивает две строки (аналог strcmp)
int strcmp(const char* a, const char* b);

// Заполняет память байтом (аналог memset)
void* memset(void* dest, int value, size_t n);

// Копирует память (аналог memcpy)
void* memcpy(void* dest, const void* src, size_t n);

#endif // STRING_H
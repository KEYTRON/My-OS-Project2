// mem.h — базовые функции работы с памятью
#ifndef MEM_H
#define MEM_H

#include <stddef.h>
#include <stdint.h>

// Копирование памяти (аналог memcpy)
void* memcpy(void* dest, const void* src, size_t n);

// Заполнение памяти байтом (аналог memset)
void* memset(void* dest, uint8_t value, size_t n);

#endif // MEM_H
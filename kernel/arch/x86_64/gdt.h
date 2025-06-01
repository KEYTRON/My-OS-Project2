// gdt.h — декларации для работы с GDT
#ifndef GDT_H
#define GDT_H

#include <stdint.h>

// Размер GDT (количество сегментов)
#define GDT_ENTRIES 6

// Индексы в массиве:
#define GDT_NULL_SEGMENT 0
#define GDT_KERNEL_CODE_SEGMENT 1
#define GDT_KERNEL_DATA_SEGMENT 2
#define GDT_USER_CODE_SEGMENT 3
#define GDT_USER_DATA_SEGMENT 4
#define GDT_TSS_SEGMENT 5

// Структура одного описателя GDT (8 байт)
struct GDTEntry {
    uint16_t limit_low;     // Младшие 16 бит лимита
    uint16_t base_low;      // Младшие 16 бит базового адреса
    uint8_t base_middle;    // Биты 16..23 базового адреса
    uint8_t access;         // Биты доступа (present, DPL, S, тип сегмента и т. д.)
    uint8_t granularity;    // Флаги гранулярности и оставшиеся биты лимита
    uint8_t base_high;      // Биты 24..31 базового адреса
} __attribute__((packed));

// Структура указателя GDTR (6 байт)
struct GDTPtr {
    uint16_t limit;         // размер GDT минус 1
    uint64_t base;          // базовый адрес GDT
} __attribute__((packed));

// Функция инициализации GDT
void gdt_init();

#endif // GDT_H
// gdt.c — реализация GDT для x86_64
#include "gdt.h"
#include "../../lib/string.h"

// Определяем массив GDT из 6 записей
static struct GDTEntry gdt_entries[GDT_ENTRIES];
static struct GDTPtr   gdt_ptr;

// Вспомогательная функция для установки одного дескриптора
static void set_gdt_entry(int idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[idx].limit_low    = (limit & 0xFFFF);
    gdt_entries[idx].base_low     = (base & 0xFFFF);
    gdt_entries[idx].base_middle  = (base >> 16) & 0xFF;
    gdt_entries[idx].access       = access;
    gdt_entries[idx].granularity  = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt_entries[idx].base_high    = (base >> 24) & 0xFF;
}

// Внешняя функция (реализуется в ассемблере) для загрузки GDTR
extern void load_gdt(void*);

// Инициализация GDT
void gdt_init() {
    // Обнуляем массив (на всякий случай)
    memset(&gdt_entries, 0, sizeof(gdt_entries));

    // 1) NULL дескриптор
    set_gdt_entry(GDT_NULL_SEGMENT, 0, 0, 0, 0);

    // 2) Кодовый сегмент ядра (0x08)
    // base=0, limit=0xFFFFF (4GB), access=0x9A (present, ring 0, код), gran=0xA0 (4K, 64-бит)
    set_gdt_entry(GDT_KERNEL_CODE_SEGMENT, 0x0, 0x000FFFFF, 0x9A, 0xA0);

    // 3) Данные сегмент ядра (0x10)
    // access=0x92 (present, ring 0, данные), gran=0xA0
    set_gdt_entry(GDT_KERNEL_DATA_SEGMENT, 0x0, 0x000FFFFF, 0x92, 0xA0);

    // 4) Кодовый сегмент пользователя (0x18)
    set_gdt_entry(GDT_USER_CODE_SEGMENT, 0x0, 0x000FFFFF, 0xFA, 0xA0);

    // 5) Данные сегмент пользователя (0x20)
    set_gdt_entry(GDT_USER_DATA_SEGMENT, 0x0, 0x000FFFFF, 0xF2, 0xA0);

    // 6) TSS (скелет, не будем пока заполнять)
    set_gdt_entry(GDT_TSS_SEGMENT, 0x0, 0x00000000, 0x89, 0x00);

    // Настраиваем указатель GDTR
    gdt_ptr.limit = sizeof(gdt_entries) - 1;
    gdt_ptr.base  = (uint64_t)&gdt_entries;

    // Вызываем ассемблерную функцию, которая выполнит lgdt [gdt_ptr]
    load_gdt(&gdt_ptr);
}
// idt.h — декларации для работы с IDT (Interrupt Descriptor Table)
#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT_ENTRIES 256

// Структура одного дескриптора IDT (16 байт)
struct IDTEntry {
    uint16_t offset_low;    // биты 0..15 смещения обработчика
    uint16_t selector;      // селектор сегмента кода (обычно 0x08)
    uint8_t ist;            // interrupt stack table offset (или zero)
    uint8_t type_attr;      // тип и атрибуты (present, DPL, тип — interrupt/gate)
    uint16_t offset_middle; // биты 16..31 смещения обработчика
    uint32_t offset_high;   // биты 32..63 смещения обработчика
    uint32_t zero;          // зарезервировано, ноль
} __attribute__((packed));

// Указатель IDTR
struct IDTPtr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// Инициализация IDT
void idt_init();

// Регистрация обработчиков
void register_interrupt_handler(int n, void (*handler)());

#endif // IDT_H
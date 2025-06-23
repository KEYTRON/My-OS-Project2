

#include "interrupts.h"

// Структура записи IDT
struct IDTEntry {
    unsigned short offset_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short offset_high;
} __attribute__((packed));

// Указатель на IDT
struct IDTPointer {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

static struct IDTEntry idt[256];
static struct IDTPointer idt_ptr;

// Заглушка для загрузки IDT (реализация в ASM позже)
extern void load_idt(unsigned int);

void init_interrupts() {
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (unsigned int)&idt;

    // Запишем таблицу IDT в процессор
    load_idt((unsigned int)&idt_ptr);
}
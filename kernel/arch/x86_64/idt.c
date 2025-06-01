// idt.c — реализация IDT
#include "idt.h"
#include <string.h>

extern void load_idt(void*);  // Ассемблерная функция, выполняющая lidt [rdi]

// Массив дескрипторов
static struct IDTEntry idt_entries[IDT_ENTRIES];
static struct IDTPtr   idt_ptr;

// Вспомогательная функция для установки одного дескриптора
static void set_idt_entry(int idx, uint64_t offset, uint16_t selector, uint8_t type_attr) {
    idt_entries[idx].offset_low    = offset & 0xFFFF;
    idt_entries[idx].selector      = selector;
    idt_entries[idx].ist           = 0;
    idt_entries[idx].type_attr     = type_attr;
    idt_entries[idx].offset_middle = (offset >> 16) & 0xFFFF;
    idt_entries[idx].offset_high   = (offset >> 32) & 0xFFFFFFFF;
    idt_entries[idx].zero          = 0;
}

// Массив указателей на функции-обработчики (ISR)
void (*interrupt_handlers[IDT_ENTRIES])();

// Регистрация обработчика
void register_interrupt_handler(int n, void (*handler)()) {
    interrupt_handlers[n] = handler;
}

// Инициализационная функция
void idt_init() {
    // Обнуляем таблицу
    memset(&idt_entries, 0, sizeof(idt_entries));

    // По умолчанию заполняем каждый дескриптор указателем на ru_stub, если нет регистраций
    for (int i = 0; i < IDT_ENTRIES; i++) {
        // Здесь мы можем сделать так, чтобы все неприсвоенные исключения шли в общую обработку
        set_idt_entry(i, (uint64_t)0, 0x08, 0x8E); // временно нулевой offset, будем заполнять позже
    }

    // Здесь можно прописать истинные адреса обработчиков (из isr.c), например:
    extern void isr_stub_table(); // массив ассемблерных заглушек, вызывающих C‑функцию
    for (int i = 0; i < 32; i++) {
        set_idt_entry(i, (uint64_t)(&isr_stub_table + i * 16), 0x08, 0x8E);
    }

    // Таймер (IRQ0) будет на 32 номер (0x20)
    extern void irq0_handler_stub();
    set_idt_entry(32, (uint64_t)irq0_handler_stub, 0x08, 0x8E);

    // Клавиатурный IRQ (IRQ1 → 33)
    extern void irq1_handler_stub();
    set_idt_entry(33, (uint64_t)irq1_handler_stub, 0x08, 0x8E);

    // И так далее можно прописать остальные IRQ...

    // Настраиваем указатель IDTR
    idt_ptr.limit = sizeof(idt_entries) - 1;
    idt_ptr.base  = (uint64_t)&idt_entries;

    // Вызываем ассемблерную функцию, которая выполнит lidt [idt_ptr]
    load_idt(&idt_ptr);
}
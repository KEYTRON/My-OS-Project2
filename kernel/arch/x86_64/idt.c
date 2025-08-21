// idt.c — реализация IDT
#include "idt.h"
#include "../../lib/string.h"

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

// Вспомогательная функция для отправки команд в порты
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Ремап PIC (Programmable Interrupt Controller)
static void remap_pic() {
    // ICW1: начинаем инициализацию
    outb(0x20, 0x11);  // мастер PIC
    outb(0xA0, 0x11);  // слейв PIC
    
    // ICW2: ремап IRQ на векторы 32-47
    outb(0x21, 0x20);  // мастер: IRQ 0-7 -> векторы 32-39
    outb(0xA1, 0x28);  // слейв: IRQ 8-15 -> векторы 40-47
    
    // ICW3: указываем каскад
    outb(0x21, 0x04);  // мастер: IRQ2 подключен слейв
    outb(0xA1, 0x02);  // слейв: ID = 2
    
    // ICW4: режим работы
    outb(0x21, 0x01);  // мастер: 8086 режим
    outb(0xA1, 0x01);  // слейв: 8086 режим
    
    // Маскируем все IRQ кроме клавиатуры (IRQ1)
    outb(0x21, 0xFD);  // мастер: разрешаем только IRQ1 (клавиатура)
    outb(0xA1, 0xFF);  // слейв: маскируем все
}

// Инициализационная функция
void idt_init() {
    // Обнуляем таблицу
    memset(&idt_entries, 0, sizeof(idt_entries));

    // Ремап PIC
    remap_pic();

    // Устанавливаем ISR заглушки (0-31)
    extern void isr0(), isr1(), isr2(), isr3(), isr4(), isr5(), isr6(), isr7();
    extern void isr8(), isr9(), isr10(), isr11(), isr12(), isr13(), isr14(), isr15();
    extern void isr16(), isr17(), isr18(), isr19(), isr20(), isr21(), isr22(), isr23();
    extern void isr24(), isr25(), isr26(), isr27(), isr28(), isr29(), isr30(), isr31();
    
    void* isr_stubs[] = {
        isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7,
        isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15,
        isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
        isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31
    };
    
    for (int i = 0; i < 32; i++) {
        set_idt_entry(i, (uint64_t)isr_stubs[i], 0x08, 0x8E);
    }

    // Устанавливаем IRQ заглушки (32-47)
    extern void irq0(), irq1(), irq2(), irq3(), irq4(), irq5(), irq6(), irq7();
    extern void irq8(), irq9(), irq10(), irq11(), irq12(), irq13(), irq14(), irq15();
    
    void* irq_stubs[] = {
        irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7,
        irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15
    };
    
    for (int i = 0; i < 16; i++) {
        set_idt_entry(32 + i, (uint64_t)irq_stubs[i], 0x08, 0x8E);
    }

    // Настраиваем указатель IDTR
    idt_ptr.limit = sizeof(idt_entries) - 1;
    idt_ptr.base  = (uint64_t)&idt_entries;

    // Вызываем ассемблерную функцию, которая выполнит lidt [idt_ptr]
    load_idt(&idt_ptr);
}
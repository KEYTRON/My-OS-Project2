// isr.c — обработчики прерываний
#include "isr.h"
#include "../../drivers/serial.h"
#include "../../drivers/vga.h"
#include "../../lib/printf.h"

// Массив с сообщениями об исключениях CPU (0..31)
const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

// Общий обработчик исключений
void isr_handler(registers_t* regs) {
    // Остановим таймер или задачи, и выведем сообщение
    // Используем упрощённый printf
    printf("Received Interrupt: %d\n", regs->int_no);
    if (regs->int_no < 32) {
        printf("Exception: %s\n", exception_messages[regs->int_no]);
    }
    // Здесь можно «парковать» ядро в бесконечности:
    while (1) {
        asm volatile("hlt");
    }
}

// Общий обработчик IRQ
void irq_handler(registers_t* regs) {
    if (regs->int_no == 32) {
        // обработка таймера
        // можно увеличить счетчик «тик-тайма» и переключить задачи
        printf("Timer interrupt!\n");
    } else if (regs->int_no == 33) {
        // обработка клавиатуры (scan code в IO port 0x60)
        uint8_t scancode;
        asm volatile("inb %1, %0" : "=a"(scancode) : "Nd"(0x60));
        // здесь можно переводить scancode в символ
        printf("Keyboard interrupt! Scancode: %x\n", scancode);
    }

    // Отправляем EOI (End Of Interrupt) контроллеру PIC
    if (regs->int_no >= 40) {
        // Слейв PIC
        asm volatile("movb $0x20, %%al; outb %%al, $0xA0" : : : "al");
    }
    // Мастер PIC (всегда)
    asm volatile("movb $0x20, %%al; outb %%al, $0x20" : : : "al");
}
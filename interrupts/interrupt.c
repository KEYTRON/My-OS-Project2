// interrupt.c
#include "interrupt.h"
#include "isr.h"

void init_interrupts() {
    // Инициализация прерываний, установка обработчиков
    for (int i = 0; i < 256; i++) {
        register_interrupt_handler(i, default_interrupt_handler);
    }

    // Инициализация таймеров, клавиатуры и других прерываний
    init_timer();
    init_keyboard();
}

void default_interrupt_handler(struct regs *r) {
    print_string("Interrupt triggered: ");
    print_int(r->int_no);
    print_string("\n");
}

void register_interrupt_handler(int interrupt, interrupt_handler_t handler) {
    interrupt_handlers[interrupt] = handler;
}
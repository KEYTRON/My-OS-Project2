// isr.c
#include "isr.h"

interrupt_handler_t interrupt_handlers[256];

// Обработчик прерываний
void isr_handler(struct regs *r) {
    if (interrupt_handlers[r->int_no] != 0) {
        interrupt_handlers[r->int_no](r);
    } else {
        print_string("Unhandled interrupt: ");
        print_int(r->int_no);
        print_string("\n");
    }
}
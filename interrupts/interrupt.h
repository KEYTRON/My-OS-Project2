// interrupt.h
#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "isr.h"

void init_interrupts();
void default_interrupt_handler(struct regs *r);
void register_interrupt_handler(int interrupt, interrupt_handler_t handler);

#endif
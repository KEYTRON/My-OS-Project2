// isr.h
#ifndef ISR_H
#define ISR_H

struct regs {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};

typedef void (*interrupt_handler_t)(struct regs *r);

extern interrupt_handler_t interrupt_handlers[256];

void isr_handler(struct regs *r);

#endif
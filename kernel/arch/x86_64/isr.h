// isr.h — объявления для исключений и IRQ
#ifndef ISR_H
#define ISR_H

#include <stdint.h>

// Структура регистра, передаваемая в обработчик исключения
typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, rsp, ss;
} registers_t;

// Функция-обработчик исключения (C‑шаблон)
void isr_handler(registers_t* regs);

// Функция-обработчик IRQ (C‑шаблон)
void irq_handler(registers_t* regs);

#endif // ISR_H
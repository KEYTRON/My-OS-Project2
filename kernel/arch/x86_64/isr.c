// isr.c — обработчики прерываний
#include "isr.h"
#include "idt.h"
#include "../../drivers/serial.h"
#include "../../drivers/vga.h"
#include "../../lib/printf.h"

// Зарегистрированные обработчики (таблица объявлена в idt.c)
extern void (*interrupt_handlers[])();

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

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint64_t read_cr2(void) {
    uint64_t value;
    asm volatile("mov %%cr2, %0" : "=r"(value));
    return value;
}

// Общий обработчик исключений: печатаем диагностику и останавливаем ядро
void isr_handler(registers_t* regs) {
    printf("\n!!! CPU EXCEPTION %d", (int)regs->int_no);
    if (regs->int_no < 32) {
        printf(" (%s)", exception_messages[regs->int_no]);
    }
    printf("\n    err=0x%lx rip=0x%lx rsp=0x%lx rflags=0x%lx\n",
           regs->err_code, regs->rip, regs->rsp, regs->rflags);
    if (regs->int_no == 14) {
        printf("    cr2=0x%lx (fault address)\n", read_cr2());
    }
    printf("System halted.\n");
    for (;;) {
        asm volatile("cli; hlt");
    }
}

// Общий обработчик IRQ: диспетчеризуем по таблице зарегистрированных
// обработчиков и посылаем EOI контроллеру PIC.
void irq_handler(registers_t* regs) {
    uint64_t n = regs->int_no;

    if (n < 256 && interrupt_handlers[n] != 0) {
        interrupt_handlers[n]();
    }

    // EOI: слейву — только для IRQ8-15, мастеру — всегда
    if (n >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}

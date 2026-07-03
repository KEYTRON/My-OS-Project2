// timer.c — драйвер системного таймера
#include "timer.h"

#if defined(__x86_64__) || defined(__amd64__)

#include "../arch/x86_64/idt.h"

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43
#define PIT_BASE_HZ  1193182u

static volatile uint64_t ticks = 0;

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static void timer_callback(void) {
    ticks++;
}

void timer_init(void) {
    uint32_t divisor = PIT_BASE_HZ / TIMER_HZ;

    // Канал 0, доступ lobyte/hibyte, режим 3 (square wave), двоичный счёт
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);

    register_interrupt_handler(32, timer_callback); // IRQ0 → вектор 32
}

uint64_t timer_ticks(void) {
    return ticks;
}

uint64_t timer_uptime_ms(void) {
    return ticks * (1000 / TIMER_HZ);
}

#else
// Заглушка для не-x86 платформ

void timer_init(void) {}

uint64_t timer_ticks(void) {
    return 0;
}

uint64_t timer_uptime_ms(void) {
    return 0;
}

#endif

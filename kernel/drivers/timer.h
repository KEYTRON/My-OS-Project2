// timer.h — драйвер системного таймера (PIT 8253/8254 на x86_64)
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define TIMER_HZ 100  // частота системного таймера

// Программируем PIT и регистрируем обработчик IRQ0
void timer_init(void);

// Количество тиков с момента старта (1 тик = 1/TIMER_HZ секунды).
// Экспортируется в Rust-часть ядра.
uint64_t timer_ticks(void);

// Миллисекунды с момента старта
uint64_t timer_uptime_ms(void);

#endif // TIMER_H

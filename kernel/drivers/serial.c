// serial.c — минимальная работа с COM-портом
#include "serial.h"
#include <stdint.h>
#include <stddef.h>

// Port I/O using inline assembly (x86_64 specific)
// Note: On non-x86 platforms or when compiling for emulation, these become no-ops

#if defined(__x86_64__) || defined(__amd64__) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)
    // Real x86 port I/O
    #define HAVE_PORT_IO 1
#else
    // No native port I/O support
    #define HAVE_PORT_IO 0
#endif

#if HAVE_PORT_IO

// Порты COM1
#define COM1_PORT 0x3F8
#define COM1_DATA COM1_PORT
#define COM1_INT_ENABLE COM1_PORT + 1
#define COM1_FIFO_CONTROL COM1_PORT + 2
#define COM1_LINE_CONTROL COM1_PORT + 3
#define COM1_MODEM_CONTROL COM1_PORT + 4
#define COM1_LINE_STATUS COM1_PORT + 5

// Отладочный порт Bochs/QEMU (debugcon). Полезно для CI, где нет доступа к VGA.
#define DEBUGCON_PORT 0xE9

// Вспомогательная функция для отправки команд в порты
static inline void outb(uint16_t port, uint8_t val) {
    // Using naked inline asm syntax compatible with both GCC and Clang
    register uint8_t al asm("al") = val;
    register uint16_t dx asm("dx") = port;
    asm volatile("outb %%al, %%dx" : : "r"(al), "r"(dx));
}

// Вспомогательная функция для чтения из портов
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    register uint16_t dx asm("dx") = port;
    asm volatile("inb %%dx, %%al" : "=a"(ret) : "r"(dx));
    return ret;
}

// Инициализация COM1 порта
void serial_init() {
    // Отключаем прерывания
    outb(COM1_INT_ENABLE, 0x00);

    // Устанавливаем скорость 38400 baud (делитель 3)
    outb(COM1_LINE_CONTROL, 0x80);  // DLAB = 1
    outb(COM1_DATA, 0x03);          // Младший байт делителя
    outb(COM1_INT_ENABLE, 0x00);    // Старший байт делителя
    outb(COM1_LINE_CONTROL, 0x03);  // DLAB = 0, 8 бит данных, 1 стоп-бит, без четности

    // Включаем FIFO
    outb(COM1_FIFO_CONTROL, 0xC7);

    // Включаем DTR, RTS и OUT2
    outb(COM1_MODEM_CONTROL, 0x0B);
}

// Отправка одного символа
void serial_write_char(char c) {
    // Ждем, пока буфер передачи не освободится, но не бесконечно (важно для CI)
    int timeout = 100000;
    while ((inb(COM1_LINE_STATUS) & 0x20) == 0 && --timeout > 0) {
        /* spin */
    }

    // Отправляем символ в COM1
    outb(COM1_DATA, c);

    // Дублируем вывод в отладочный порт. QEMU перенаправляет его в -debugcon.
    outb(DEBUGCON_PORT, c);
}

// Отправка строки
void serial_write_string(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        serial_write_char(str[i]);
    }
}

#else
// Stub implementations for non-x86 platforms

void serial_init() {
    // No-op on non-x86 platforms
}

void serial_write_char(char c) {
    (void)c;  // Suppress unused parameter warning
    // No-op on non-x86 platforms
}

void serial_write_string(const char* str) {
    (void)str;  // Suppress unused parameter warning
    // No-op on non-x86 platforms
}

#endif
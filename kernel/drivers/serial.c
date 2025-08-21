// serial.c — минимальная работа с COM-портом
#include "serial.h"
#include <stdint.h>
#include <stddef.h>

// Порты COM1
#define COM1_PORT 0x3F8
#define COM1_DATA COM1_PORT
#define COM1_INT_ENABLE COM1_PORT + 1
#define COM1_FIFO_CONTROL COM1_PORT + 2
#define COM1_LINE_CONTROL COM1_PORT + 3
#define COM1_MODEM_CONTROL COM1_PORT + 4
#define COM1_LINE_STATUS COM1_PORT + 5

// Вспомогательная функция для отправки команд в порты
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Вспомогательная функция для чтения из портов
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
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
    // Ждем, пока буфер передачи не освободится
    while ((inb(COM1_LINE_STATUS) & 0x20) == 0);
    
    // Отправляем символ
    outb(COM1_DATA, c);
}

// Отправка строки
void serial_write_string(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        serial_write_char(str[i]);
    }
}
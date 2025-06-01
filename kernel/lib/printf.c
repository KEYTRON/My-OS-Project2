// printf.c — простая реализация printf
#include "printf.h"
#include "../drivers/vga.h"
#include "../drivers/serial.h"
#include "../lib/string.h"
#include "../lib/mem.h"

// Предположим, что по умолчанию используем VGA, но можем переключаться:
#define USE_VGA
//#define USE_SERIAL_OUT

// Вспомогательная функция для записи одного символа (в VGA или Serial)
static void put_char(char c) {
#ifdef USE_SERIAL_OUT
    serial_write_char(c);
#else
    vga_putc_color(c, vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
#endif
}

// Перевод числа base‑осный в строку (рверсия itoa)
static void itoa(unsigned long value, char* str, int base) {
    char* ptr = str, *ptr1 = str, tmp_char;
    unsigned long tmp_value;

    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return;
    }

    while (value != 0) {
        tmp_value = value % base;
        if (tmp_value < 10) {
            *ptr++ = tmp_value + '0';
        } else {
            *ptr++ = tmp_value - 10 + 'a';
        }
        value /= base;
    }

    // Обратный порядок
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}

// Основная функция printf
void printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    const char* traverse;
    char buffer[32];

    for (traverse = format; *traverse != '\0'; traverse++) {
        if (*traverse != '%') {
            put_char(*traverse);
            continue;
        }
        traverse++;
        // Спецификатор:
        switch (*traverse) {
            case 'c': {
                char c = (char)va_arg(args, int);
                put_char(c);
            } break;
            case 'd': {
                int i = va_arg(args, int);
                if (i < 0) {
                    put_char('-');
                    i = -i;
                }
                itoa(i, buffer, 10);
                for (char* p = buffer; *p; p++) put_char(*p);
            } break;
            case 'u': {
                unsigned int u = va_arg(args, unsigned int);
                itoa(u, buffer, 10);
                for (char* p = buffer; *p; p++) put_char(*p);
            } break;
            case 'x': {
                unsigned int x = va_arg(args, unsigned int);
                itoa(x, buffer, 16);
                for (char* p = buffer; *p; p++) put_char(*p);
            } break;
            case 's': {
                const char* s = va_arg(args, const char*);
                for (size_t i = 0; s[i]; i++) put_char(s[i]);
            } break;
            case '%': {
                put_char('%');
            } break;
            default: {
                put_char('%');
                put_char(*traverse);
            } break;
        }
    }

    va_end(args);
}
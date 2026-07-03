// printf.c — простая реализация printf (VGA + serial)
#include "printf.h"
#include "../drivers/vga.h"
#include "../drivers/serial.h"
#include "../lib/string.h"

// Вывод идёт одновременно на VGA и в COM1/debugcon — так лог виден и на
// экране, и в CI (qemu -serial/-debugcon).
static void put_char(char c) {
    if (c == '\f') { // form feed = очистка экрана (используется shell'ом)
        vga_clear();
        return;
    }
    vga_putc_color(c, vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    serial_write_char(c);
}

// Экспортируемый посимвольный вывод (используется Rust-частью ядра)
void kputchar(char c) {
    put_char(c);
}

// Перевод беззнакового числа в строку в указанной системе счисления
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

static void put_string(const char* s) {
    while (*s) put_char(*s++);
}

// Основная функция printf. Поддерживает %c %d %u %x %s %p %%
// и длинный модификатор l (%ld %lu %lx).
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

        int is_long = 0;
        while (*traverse == 'l') { // %l, %ll — трактуем одинаково (64 бита)
            is_long = 1;
            traverse++;
        }
        if (*traverse == 'z') { // %zu и т.п.
            is_long = 1;
            traverse++;
        }

        switch (*traverse) {
            case 'c': {
                char c = (char)va_arg(args, int);
                put_char(c);
            } break;
            case 'd': case 'i': {
                long i = is_long ? va_arg(args, long) : (long)va_arg(args, int);
                if (i < 0) {
                    put_char('-');
                    i = -i;
                }
                itoa((unsigned long)i, buffer, 10);
                put_string(buffer);
            } break;
            case 'u': {
                unsigned long u = is_long ? va_arg(args, unsigned long)
                                          : (unsigned long)va_arg(args, unsigned int);
                itoa(u, buffer, 10);
                put_string(buffer);
            } break;
            case 'x': {
                unsigned long x = is_long ? va_arg(args, unsigned long)
                                          : (unsigned long)va_arg(args, unsigned int);
                itoa(x, buffer, 16);
                put_string(buffer);
            } break;
            case 'p': {
                unsigned long p = (unsigned long)va_arg(args, void*);
                put_string("0x");
                itoa(p, buffer, 16);
                put_string(buffer);
            } break;
            case 's': {
                const char* s = va_arg(args, const char*);
                if (!s) s = "(null)";
                put_string(s);
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

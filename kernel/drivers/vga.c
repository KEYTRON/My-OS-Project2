// vga.c — работа с текстовым буфером VGA
#include "vga.h"
#include <stddef.h>
#include <stdint.h>

// По физическому адресу 0xB8000 расположен текстовый буфер VGA
static uint16_t* const VGA_BUFFER = (uint16_t*)0xB8000;
static uint8_t terminal_row = 0;
static uint8_t terminal_col = 0;
static uint8_t terminal_color;

#if defined(__x86_64__) || defined(__amd64__)
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}
#else
static inline void outb(uint16_t port, uint8_t val) { (void)port; (void)val; }
#endif

// Установить цвет (фоновый + символа)
uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
    return fg | bg << 4;
}

// Создать 16-битную ячейку: символ + цвет
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t)uc | (uint16_t)color << 8;
}

// Обновить положение аппаратного курсора
static void vga_update_cursor(void) {
    uint16_t pos = terminal_row * VGA_WIDTH + terminal_col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

// Инициализация (очистка экрана)
void vga_init() {
    terminal_row = 0;
    terminal_col = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            VGA_BUFFER[index] = vga_entry(' ', terminal_color);
        }
    }
    vga_update_cursor();
}

void vga_clear(void) {
    vga_init();
}

// Прокрутка экрана на одну строку вверх
static void vga_scroll(void) {
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[(y - 1) * VGA_WIDTH + x] = VGA_BUFFER[y * VGA_WIDTH + x];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }
}

// Вывод символа
void vga_putc_color(char c, uint8_t color) {
    if (c == '\n') {
        terminal_col = 0;
        terminal_row++;
    } else if (c == '\r') {
        terminal_col = 0;
    } else if (c == '\b') {
        if (terminal_col > 0) {
            terminal_col--;
        } else if (terminal_row > 0) {
            terminal_row--;
            terminal_col = VGA_WIDTH - 1;
        }
        VGA_BUFFER[terminal_row * VGA_WIDTH + terminal_col] = vga_entry(' ', color);
    } else if (c == '\t') {
        do {
            vga_putc_color(' ', color);
        } while (terminal_col % 4 != 0 && terminal_col < VGA_WIDTH);
        return;
    } else {
        const size_t index = terminal_row * VGA_WIDTH + terminal_col;
        VGA_BUFFER[index] = vga_entry(c, color);
        terminal_col++;
        if (terminal_col >= VGA_WIDTH) {
            terminal_col = 0;
            terminal_row++;
        }
    }
    if (terminal_row >= VGA_HEIGHT) {
        vga_scroll();
        terminal_row = VGA_HEIGHT - 1;
    }
    vga_update_cursor();
}

// Вывод строки
void vga_write_string(const char* str, uint8_t color) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        vga_putc_color(str[i], color);
    }
}

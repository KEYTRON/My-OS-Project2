// vga.c — работа с текстовым буфером VGA
#include "vga.h"
#include <stddef.h>
#include <stdint.h>

// По физическому адресу 0xB8000 расположен текстовый буфер VGA
static uint16_t* const VGA_BUFFER = (uint16_t*)0xB8000;
static uint8_t terminal_row = 0;
static uint8_t terminal_col = 0;
static uint8_t terminal_color;

// Установить цвет (фоновый + символа)
static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
    return fg | bg << 4;
}

// Создать 16‑битную ячейку: символ + цвет
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t)uc | (uint16_t)color << 8;
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
}

// Вывод символа
void vga_putc_color(char c, uint8_t color) {
    if (c == '\n') {
        terminal_col = 0;
        terminal_row++;
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
        // Если дошли до конца экрана, просто "заливаем" снизу или сбрасываем на 0
        terminal_row = 0;
    }
}

// Вывод строки
void vga_write_string(const char* str, uint8_t color) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        vga_putc_color(str[i], color);
    }
}
#include "vga.h"
#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define WHITE_ON_BLACK 0x07

static int cursor_row = 0;
static int cursor_col = 0;

static char *const video = (char*)VGA_ADDRESS;

void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = WHITE_ON_BLACK;
    }
    cursor_row = 0;
    cursor_col = 0;
}

void put_char(char c) {
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
        return;
    }

    int index = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video[index] = c;
    video[index + 1] = WHITE_ON_BLACK;
    cursor_col++;
    if (cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        cursor_row++;
    }
}

void print_string(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        put_char(str[i]);
    }
}

// === Графические функции (320x200x256 VGA mode) ===

void put_pixel(int x, int y, unsigned char color) {
    unsigned char *vga = (unsigned char*)0xA0000;
    unsigned int offset = y * 320 + x;
    vga[offset] = color;
}

void clear_graphics(unsigned char color) {
    unsigned char *vga = (unsigned char*)0xA0000;
    for (int i = 0; i < 320 * 200; i++) {
        vga[i] = color;
    }
}

}
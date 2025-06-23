

#include "vga.h"

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define WHITE_ON_BLACK 0x07

void clear_screen() {
    char *video = (char*) VGA_ADDRESS;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = WHITE_ON_BLACK;
    }
}
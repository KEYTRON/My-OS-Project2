// video.c
#include "video.h"

#define VIDEO_MEMORY 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

static char *video_memory = (char *)VIDEO_MEMORY;

void print_char(char c, int x, int y) {
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return;
    video_memory[(y * SCREEN_WIDTH + x) * 2] = c;
    video_memory[(y * SCREEN_WIDTH + x) * 2 + 1] = 0x07;  // Цвет текста
}

void print_string(const char *str) {
    int x = 0, y = 0;
    while (*str) {
        if (*str == '\n') {
            y++;
            x = 0;
        } else {
            print_char(*str, x, y);
            x++;
        }
        str++;
    }
}
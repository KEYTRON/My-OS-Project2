// screen.c
#include "screen.h"

// Адрес видеопамяти для текстового режима
static unsigned short *video_memory = (unsigned short *) 0xB8000;

void print_char(char c, int x, int y) {
    unsigned short *location = video_memory + (y * 80 + x);
    *location = (0x07 << 8) | c;  // Цвет текста (0x07 - серый на черном фоне)
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
            if (x >= 80) {
                x = 0;
                y++;
            }
        }
        str++;
    }
}
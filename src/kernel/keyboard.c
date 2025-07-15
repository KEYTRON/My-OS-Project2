#include "keyboard.h"
#include "vga.h"

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static const char keymap[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
    'a','s','d','f','g','h','j','k','l',';','\'', '`',0,'\\','z','x','c',
    'v','b','n','m',',','.','/',0,'*',0,' ',0,
};

void init_keyboard() {
    print_string("Keyboard initialized.\n");
}

static char read_scancode() {
    unsigned char status;
    do {
        status = inb(0x64);
    } while (!(status & 1));
    return inb(0x60);
}

char keyboard_get_char() {
    char c = 0;
    while (!c) {
        char sc = read_scancode();
        c = keymap[(unsigned char)sc];
    }
    return c;
}

void read_line(char *buf, int max_len) {
    int i = 0;
    while (i < max_len - 1) {
        char c = keyboard_get_char();
        if (c == '\n') {
            buf[i] = '\0';
            put_char('\n');
            return;
        }
        buf[i++] = c;
        put_char(c);
    }
    buf[i] = '\0';
}

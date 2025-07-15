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
    print_string("\nKeyboard initialized. Type keys:\n");
    while (1) {
        unsigned char status = inb(0x64);
        if (status & 1) {
            unsigned char sc = inb(0x60);
            char c = keymap[sc];
            if (c) {
                put_char(c);
            }
        }
    }
}

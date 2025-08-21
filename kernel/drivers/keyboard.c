// keyboard.c — простой обработчик клавиатуры
#include "keyboard.h"
#include "../arch/x86_64/isr.h"
#include "../arch/x86_64/idt.h"
#include "../drivers/vga.h"
#include "../lib/printf.h"

#define PORT_KEYDATA 0x60
#define PORT_KEYSTATUS 0x64

char keymap[128] = {
    0,  27, '1','2','3','4','5','6', '7','8','9','0','-','=', '\b','\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n', 0, 'a','s',
    'd','f','g','h','j','k','l',';','\'','`',  0, '\\','z','x','c','v',
    'b','n','m',',','.','/',   0, '*',  0, ' ',  0,   0,   0,   0,   0,
    /* далее  далее */ 0
};

// Инициализация: регистрируем обработчик IRQ1 в IDT
void keyboard_init() {
    register_interrupt_handler(33, keyboard_callback); // IRQ1 → 33 (32 + 1)
}

// Функция-обработчик: считываем scancode и печатаем на экран
void keyboard_callback() {
    uint8_t scancode;
    asm volatile("inb %1, %0" : "=a"(scancode) : "Nd"(PORT_KEYDATA));

    if (scancode < 128) {
        char c = keymap[scancode];
        if (c) {
            // Печатаем символ на VGA
            vga_putc_color(c, vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
        }
    }

    // EOI для PIC
    asm volatile("movb $0x20, %%al; outb %%al, $0x20" : : : "al");
}

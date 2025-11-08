// keyboard.c — простой обработчик клавиатуры
#include "keyboard.h"

#if defined(__x86_64__) || defined(__amd64__) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)
// Real x86 keyboard handling
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
static void inline_keyboard_read(uint8_t *scancode) {
    // Read from port 0x60 using inline assembly
    unsigned char val;
    asm volatile("inb $0x60, %0" : "=a" (val));
    *scancode = val;
}

static void inline_keyboard_eoi(void) {
    // Send EOI (End of Interrupt) to PIC
    asm volatile("outb %0, $0x20" : : "a" ((unsigned char)0x20));
}

void keyboard_callback() {
    uint8_t scancode = 0;
    inline_keyboard_read(&scancode);

    if (scancode < 128) {
        char c = keymap[scancode];
        if (c) {
            // Печатаем символ на VGA
            vga_putc_color(c, vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
        }
    }

    // EOI для PIC
    inline_keyboard_eoi();
}

#else
// Stub implementation for non-x86 platforms

void keyboard_init() {
    // No-op on non-x86 platforms
}

void keyboard_callback() {
    // No-op on non-x86 platforms
}

#endif

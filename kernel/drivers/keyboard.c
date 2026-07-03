// keyboard.c — драйвер PS/2 клавиатуры с кольцевым буфером ввода
#include "keyboard.h"

#if defined(__x86_64__) || defined(__amd64__) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)
// Real x86 keyboard handling
#include "../arch/x86_64/isr.h"
#include "../arch/x86_64/idt.h"

#define PORT_KEYDATA 0x60

// scancode set 1 → ASCII (без Shift)
char keymap[128] = {
    0,  27, '1','2','3','4','5','6', '7','8','9','0','-','=', '\b','\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n', 0, 'a','s',
    'd','f','g','h','j','k','l',';','\'','`',  0, '\\','z','x','c','v',
    'b','n','m',',','.','/',   0, '*',  0, ' ',  0,   0,   0,   0,   0,
    0
};

// scancode set 1 → ASCII (с Shift)
static const char keymap_shift[128] = {
    0,  27, '!','@','#','$','%','^', '&','*','(',')','_','+', '\b','\t',
    'Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0, 'A','S',
    'D','F','G','H','J','K','L',':','"','~',  0, '|','Z','X','C','V',
    'B','N','M','<','>','?',   0, '*',  0, ' ',  0,   0,   0,   0,   0,
    0
};

#define KBD_BUFFER_SIZE 256

static volatile char kbd_buffer[KBD_BUFFER_SIZE];
static volatile uint32_t kbd_head = 0;  // куда пишет обработчик
static volatile uint32_t kbd_tail = 0;  // откуда читает потребитель

static uint8_t shift_pressed = 0;
static uint8_t ctrl_pressed = 0;
static uint8_t caps_lock = 0;

static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    asm volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

// Обработчик IRQ1: переводим scancode в символ и кладём в буфер.
// EOI отправляет общий irq_handler.
void keyboard_callback() {
    uint8_t scancode = inb(PORT_KEYDATA);

    // Отпускание клавиши (бит 7)
    if (scancode & 0x80) {
        uint8_t released = scancode & 0x7F;
        if (released == 0x2A || released == 0x36) shift_pressed = 0;
        if (released == 0x1D) ctrl_pressed = 0;
        return;
    }

    switch (scancode) {
        case 0x2A: case 0x36: shift_pressed = 1; return; // Shift
        case 0x1D: ctrl_pressed = 1; return;             // Ctrl
        case 0x3A: caps_lock ^= 1; return;               // Caps Lock
        default: break;
    }

    if (scancode >= 128) return;

    char c = shift_pressed ? keymap_shift[scancode] : keymap[scancode];
    if (!c) return;

    // Caps Lock влияет только на буквы
    if (caps_lock && !shift_pressed && c >= 'a' && c <= 'z') c -= 32;
    else if (caps_lock && shift_pressed && c >= 'A' && c <= 'Z') c += 32;

    // Ctrl+буква → управляющий код (Ctrl+C = 3 и т.п.)
    if (ctrl_pressed && (c >= 'a' && c <= 'z')) c = c - 'a' + 1;

    uint32_t next = (kbd_head + 1) % KBD_BUFFER_SIZE;
    if (next != kbd_tail) { // при переполнении символ теряется
        kbd_buffer[kbd_head] = c;
        kbd_head = next;
    }
}

void keyboard_init() {
    kbd_head = kbd_tail = 0;
    register_interrupt_handler(33, keyboard_callback); // IRQ1 → вектор 33
}

// Неблокирующее чтение символа из буфера: -1, если буфер пуст.
// Экспортируется в Rust-часть ядра.
int keyboard_getchar(void) {
    if (kbd_tail == kbd_head) return -1;
    char c = kbd_buffer[kbd_tail];
    kbd_tail = (kbd_tail + 1) % KBD_BUFFER_SIZE;
    return (int)(unsigned char)c;
}

#else
// Stub implementation for non-x86 platforms

void keyboard_init() {}

void keyboard_callback() {}

int keyboard_getchar(void) {
    return -1;
}

#endif

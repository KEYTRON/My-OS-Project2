// keyboard.c
#include "keyboard.h"
#include "../io/screen.h"

#define KEYBOARD_PORT 0x60

unsigned char get_scancode() {
    return inb(KEYBOARD_PORT);  // Чтение байта с порта клавиатуры
}

void keyboard_handler() {
    unsigned char scancode = get_scancode();
    
    // Преобразование скан-кодов в символы (очень простая версия)
    if (scancode == 0x1E) {
        print_string("A");
    } else if (scancode == 0x30) {
        print_string("B");
    }
    // Добавить другие скан-коды по мере необходимости
}

void init_keyboard() {
    // Инициализация клавиатуры, подключение обработчика прерывания
    register_interrupt_handler(IRQ1, keyboard_handler);  // IRQ1 - клавиатура
}
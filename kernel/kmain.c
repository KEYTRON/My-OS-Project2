// kmain.c — основная точка входа в C‑код ядра
#include <stdint.h>
#include "include/common.h"
#include "arch/x86_64/gdt.h"
#include "arch/x86_64/idt.h"
#include "arch/x86_64/paging.h"
#include "drivers/vga.h"
#include "drivers/serial.h"
#include "drivers/keyboard.h"
#include "lib/printf.h"

// Функция, вызываемая из entry.S
void kernel_main() {
    // Для начала можно поставить громкую точку: ядро запустилось
    // Настроим видеовывод (VGA)
    vga_init();
    printf("Kernel: Запуск ядра...\n");

    // Настроим serial (COM1) для отладки
    serial_init();
    serial_write_string("Serial initialized.\n");

    // Установим GDT
    gdt_init();
    printf("GDT initialized.\n");
    serial_write_string("GDT initialized.\n");

    // Настраиваем IDT и регистрируем обработчики
    idt_init();
    printf("IDT initialized.\n");
    serial_write_string("IDT initialized.\n");

    // Включаем пейджинг (виртуальная память)
    paging_init();
    printf("Paging initialized.\n");
    serial_write_string("Paging initialized.\n");

    // Инициализируем клавиатуру
    keyboard_init();
    printf("Keyboard driver initialized.\n");
    serial_write_string("Keyboard driver initialized.\n");

    // Включаем прерывания
    asm volatile("sti");

    // Приветствие
    printf("\nПривет! Это минимальное ядро на С/С++.\n");
    serial_write_string("Kernel says hello!\n");

    // Теперь можно запустить основной цикл
    while (1) {
        asm volatile("hlt"); // при отсутствии задач простаиваем, пока прерывание не придёт
    }
}
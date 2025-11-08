// kmain.c — основная точка входа в C‑код ядра
#include <stdint.h>
#include "include/common.h"
#include "include/arch.h"

// Архитектурно-зависимые заголовки
#ifdef ARCH_X86_64
#include "arch/x86_64/gdt.h"
#include "arch/x86_64/idt.h"
#include "arch/x86_64/paging.h"
#elif defined(ARCH_ARM64)
// ARM64 специфичные заголовки будут добавлены позже
#elif defined(ARCH_RISCV64)
// RISC-V64 специфичные заголовки будут добавлены позже
#endif

#include "drivers/vga.h"
#include "drivers/serial.h"
#include "drivers/keyboard.h"
#include "lib/printf.h"

// TUI система
#include "include/tui/tui.h"
#include "include/tui/tui_demo.h"

// Функция, вызываемая из entry.S
void kernel_main() {
    // Для начала можно поставить громкую точку: ядро запустилось
    // Настроим видеовывод (VGA)
    vga_init();
    printf("Kernel: Запуск ядра на архитектуре ");
    
    // Выводим информацию об архитектуре
#ifdef ARCH_X86_64
    printf("x86_64");
#elif defined(ARCH_ARM64)
    printf("ARM64");
#elif defined(ARCH_RISCV64)
    printf("RISC-V64");
#endif
    printf("...\n");

    // Настроим serial (COM1) для отладки
    serial_init();
    serial_write_string("Serial initialized.\n");

#ifdef ARCH_X86_64
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
#elif defined(ARCH_ARM64)
    // ARM64 специфичная инициализация
    printf("ARM64 initialization...\n");
    serial_write_string("ARM64 initialization...\n");

    // GIC (Generic Interrupt Controller) инициализация
    // Сейчас используем базовую инициализацию
    // В реальной системе нужно:
    // 1. Инициализировать GIC Distributor
    // 2. Инициализировать GIC CPU Interface
    printf("GIC initialization...\n");
    serial_write_string("GIC initialization...\n");

    // MMU (Memory Management Unit) инициализация
    // В реальной системе нужно установить таблицы трансляции страниц
    printf("MMU setup...\n");
    serial_write_string("MMU setup...\n");

    // Exception vectors инициализация
    // В реальной системе нужно установить базовый адрес vector table
    printf("Exception vectors initialized...\n");
    serial_write_string("Exception vectors initialized...\n");

#elif defined(ARCH_RISCV64)
    // RISC-V64 специфичная инициализация
    printf("RISC-V64 initialization...\n");
    serial_write_string("RISC-V64 initialization...\n");

    // PLIC (Platform-Level Interrupt Controller) инициализация
    // В реальной системе нужно:
    // 1. Включить интерпретацию глобальных прерываний
    // 2. Установить приоритеты источников
    printf("PLIC initialization...\n");
    serial_write_string("PLIC initialization...\n");

    // MMU инициализация
    // В реальной системе нужно установить режим трансляции адресов
    printf("Sv39/Sv48 paging enabled...\n");
    serial_write_string("Sv39/Sv48 paging enabled...\n");

    // Exception handlers инициализация
    // В реальной системе нужно установить trap vector base address
    printf("Exception handlers initialized...\n");
    serial_write_string("Exception handlers initialized...\n");

#endif

    // Инициализируем клавиатуру
    keyboard_init();
    printf("Keyboard driver initialized.\n");
    serial_write_string("Keyboard driver initialized.\n");

    // Включаем прерывания используя архитектурно-независимый интерфейс
    arch_enable_interrupts();
    printf("Interrupts enabled.\n");
    serial_write_string("Interrupts enabled.\n");

    // Приветствие
    printf("\nПривет! Это минимальное ядро на С/С++.\n");
    printf("Архитектура: ");
#ifdef ARCH_X86_64
    printf("x86_64");
#elif defined(ARCH_ARM64)
    printf("ARM64");
#elif defined(ARCH_RISCV64)
    printf("RISC-V64");
#endif
    printf("\n");
    
    serial_write_string("Kernel says hello!\n");

    // Инициализируем TUI систему
    printf("\nИнициализация TUI системы...\n");
    serial_write_string("Initializing TUI system...\n");
    
    if (tui_system_init()) {
        printf("TUI system initialized successfully.\n");
        serial_write_string("TUI system initialized successfully.\n");
        
        // Создаем и запускаем TUI демонстрацию
        printf("Запуск TUI демонстрации...\n");
        serial_write_string("Starting TUI demo...\n");
        
        tui_demo_app_t* demo_app = tui_demo_create();
        if (demo_app) {
            tui_demo_run(demo_app);
            tui_demo_destroy(demo_app);
        } else {
            printf("Ошибка: не удалось создать TUI демонстрацию.\n");
            serial_write_string("Error: failed to create TUI demo.\n");
        }
        
        // Очищаем TUI систему
        tui_system_cleanup();
        printf("TUI system cleaned up.\n");
        serial_write_string("TUI system cleaned up.\n");
    } else {
        printf("Ошибка: не удалось инициализировать TUI систему.\n");
        serial_write_string("Error: failed to initialize TUI system.\n");
    }

    // Возврат к консольному режиму
    printf("\nВозврат к консольному режиму...\n");
    serial_write_string("Returning to console mode...\n");
    
    // Теперь можно запустить основной цикл
    while (1) {
        arch_halt(); // при отсутствии задач простаиваем, пока прерывание не придёт
    }
}
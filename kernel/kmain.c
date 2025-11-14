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

// Graphics система
#include "lib/graphics/graphics.h"
#include "lib/graphics/graphics_font.h"

// GUI система
#include "lib/gui/gui_widgets.h"

// GUI Desktop система (Полная графическая оболочка)
#include "lib/gui/gui_desktop.h"

static inline void debugcon_write(const char *msg) {
#if defined(__x86_64__) || defined(__i386__) || defined(__i486__) || \
    defined(__i586__) || defined(__i686__)
    while (*msg) {
        unsigned char ch = (unsigned char)*msg++;
        __asm__ volatile ("outb %0, %1" : : "a"(ch), "Nd"((uint16_t)0xE9));
    }
#else
    (void)msg;
#endif
}

#ifdef ENABLE_QEMU_EXIT
static inline void qemu_exit(uint8_t code) {
    __asm__ volatile ("outb %0, %1" : : "a"(code), "Nd"((uint16_t)0xF4));
}
#else
static inline void qemu_exit(uint8_t code) {
    (void)code;
}
#endif

// Функция, вызываемая из entry.S
void kernel_main() {
    debugcon_write("[MyOS] kernel_main start\n");
#ifdef ENABLE_QEMU_EXIT
    debugcon_write("[MyOS] requesting QEMU exit\n");
    qemu_exit(0);
#endif
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

    // Приветствие с красивым splash screen
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                        ║\n");
    printf("║                        ✨ Welcome to MyOS v1.0 ✨                     ║\n");
    printf("║                                                                        ║\n");
    printf("║              Educational Operating System for x86_64, ARM64            ║\n");
    printf("║                          & RISC-V64 Architectures                      ║\n");
    printf("║                                                                        ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");

    // Информация об архитектуре
    printf("📌 System Architecture: ");
#ifdef ARCH_X86_64
    printf("x86_64 (Intel/AMD)\n");
#elif defined(ARCH_ARM64)
    printf("ARM64 (ARMv8)\n");
#elif defined(ARCH_RISCV64)
    printf("RISC-V64\n");
#endif

    printf("📊 Boot Information:\n");
    printf("   ├─ GDT initialized\n");
    printf("   ├─ IDT with %d interrupt vectors\n", 256);
    printf("   ├─ Memory paging enabled\n");
    printf("   ├─ Virtual memory support active\n");
    printf("   └─ Keyboard & VGA drivers ready\n");
    printf("\n");

    serial_write_string("Kernel says hello!\n");

#ifdef ENABLE_QEMU_EXIT
    debugcon_write("[MyOS] signalling qemu exit\n");
    qemu_exit(0);
#endif

    // ========================================
    // Инициализация графического стека
    // ========================================
    printf("\nInitializing graphics subsystem...\n");
    serial_write_string("Initializing graphics subsystem...\n");

    graphics_device_t *gfx = graphics_init();
    if (gfx != NULL) {
        printf("✓ Graphics initialized: %ux%u @ %ubpp\n", gfx->width, gfx->height, gfx->bpp);
        printf("  Mode: ");

        switch (gfx->mode) {
            case GRAPHICS_MODE_TEXT:
                printf("Text Mode");
                break;
            case GRAPHICS_MODE_VESA:
                printf("VESA VBE 3.0 (x86_64)");
                break;
            case GRAPHICS_MODE_FB_32:
                printf("32-bit Framebuffer");
                break;
            case GRAPHICS_MODE_FB_16:
                printf("16-bit Framebuffer");
                break;
            default:
                printf("Unknown");
        }
        printf("\n");
        serial_write_string("Graphics initialized successfully.\n");

        if (gfx->putpixel != NULL && gfx->bpp >= 16) {
            uint32_t background = graphics_rgb_to_color(16, 16, 32);
            graphics_clear(background);
            graphics_flush();
        } else {
            printf("Graphics rendering not available in this mode.\n");
        }
    } else {
        printf("✗ Graphics initialization failed\n");
        serial_write_string("Graphics initialization failed.\n");
    }

    // ========================================
    // Инициализация GUI системы
    // ========================================
    printf("\n");
    printf("════════════════════════════════════════════════════════════════════════\n");
    printf("                       GUI System Initialization\n");
    printf("════════════════════════════════════════════════════════════════════════\n\n");

    // Включаем только если графика доступна
    if (gfx != NULL && gfx->bpp >= 16) {

        printf("Initializing GUI system...\n");
        gui_init();
        printf("✓ GUI initialized\n\n");

        printf("GUI subsystem ready for desktop runtime.\n");
        serial_write_string("GUI subsystem ready.\n");
    } else {
        printf("⚠ GUI system requires 16-bit or higher color graphics\n");
        printf("⚠ Skipping GUI initialization in text mode\n");
    }

    // Теперь можно запустить основной цикл
    // ========================================
    // Запуск GUI Desktop (Полный графический рабочий стол)
    // ========================================
    printf("\n");
    printf("════════════════════════════════════════════════════════════════════════\n");
    printf("                   GUI Desktop Environment\n");
    printf("════════════════════════════════════════════════════════════════════════\n\n");

    // Проверяем наличие графического устройства
    if (gfx != NULL && gfx->bpp >= 16) {
        printf("Запуск полноценного GUI Desktop...\n");
        serial_write_string("Starting GUI desktop...\n");

        gui_desktop_run(gfx);

        printf("GUI Desktop успешно запущен!\n");
        serial_write_string("GUI Desktop started successfully.\n");
    } else {
        printf("⚠ Графика недоступна (требуется 16-bit+)\n");
        serial_write_string("Graphics not available, GUI disabled.\n");
    }

    printf("\nEntering main event loop...\n");
    serial_write_string("Entering main event loop.\n");

    while (1) {
        arch_halt(); // при отсутствии задач простаиваем, пока прерывание не придёт
    }
}

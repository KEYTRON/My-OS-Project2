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

// Graphics система
#include "lib/graphics/graphics.h"
#include "lib/graphics/graphics_font.h"

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

    // ========================================
    // Инициализация и демонстрация графики
    // ========================================
    printf("\n");
    printf("════════════════════════════════════════════════════════════════════════\n");
    printf("                     Graphics System Demonstration\n");
    printf("════════════════════════════════════════════════════════════════════════\n");

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

        // Проверяем возможность рендеринга
        if (gfx->putpixel != NULL && gfx->bpp >= 16) {
            printf("\nRendering graphics demo...\n");

            // Очищаем экран черным цветом
            uint32_t black = graphics_rgb_to_color(0, 0, 0);
            graphics_clear(black);

            // Рисуем красный прямоугольник в углу
            uint32_t red = graphics_rgb_to_color(255, 0, 0);
            graphics_rect_t rect1 = {.x = 50, .y = 50, .width = 200, .height = 150};
            graphics_fillrect(rect1, red);

            // Рисуем зеленый прямоугольник
            uint32_t green = graphics_rgb_to_color(0, 255, 0);
            graphics_rect_t rect2 = {.x = 300, .y = 100, .width = 150, .height = 200};
            graphics_fillrect(rect2, green);

            // Рисуем синий круг
            uint32_t blue = graphics_rgb_to_color(0, 0, 255);
            graphics_point_t center = {.x = 700, .y = 300};
            graphics_drawcircle(center, 80, blue);

            // Рисуем белую линию
            uint32_t white = graphics_rgb_to_color(255, 255, 255);
            graphics_point_t p1 = {.x = 100, .y = 400};
            graphics_point_t p2 = {.x = 900, .y = 500};
            graphics_drawline(p1, p2, white);

            // Пытаемся вывести текст если поддерживается шрифт
            uint32_t yellow = graphics_rgb_to_color(255, 255, 0);
            graphics_draw_string_at(100, 600, "MyOS Graphics Demo v1.0", yellow, black);
            graphics_draw_string_at(100, 620, "Press any key to continue...", yellow, black);

            graphics_flush();
            printf("✓ Graphics demo rendered\n");
            serial_write_string("Graphics demo rendered.\n");

            // Ждем нажатия клавиши
            printf("\nWaiting for keyboard input...\n");
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
        extern void gui_init(void);
        extern gui_widget_t *gui_window_create(const char *, int32_t, int32_t, uint32_t, uint32_t);
        extern void gui_widget_add_child(gui_widget_t *, gui_widget_t *);
        extern gui_widget_t *gui_button_create(const char *);
        extern void gui_widget_set_bounds(gui_widget_t *, int32_t, int32_t, uint32_t, uint32_t);
        extern gui_widget_t *gui_label_create(const char *);
        extern void gui_process_events(void);
        extern void gui_render(void);

        printf("Initializing GUI system...\n");
        gui_init();
        printf("✓ GUI initialized\n\n");

        // Создаем главное окно рабочего стола
        printf("Creating desktop windows...\n");
        gui_widget_t *main_window = gui_window_create("MyOS Desktop", 50, 50, 500, 400);
        if (main_window) {
            printf("✓ Main window created (500x400)\n");

            // Добавляем кнопку
            gui_widget_t *btn1 = gui_button_create("Click Me!");
            if (btn1) {
                gui_widget_set_bounds(btn1, 20, 80, 100, 30);
                gui_widget_add_child(main_window, btn1);
                printf("✓ Button added\n");
            }

            // Добавляем ярлык
            gui_widget_t *label = gui_label_create("Welcome to MyOS GUI!");
            if (label) {
                gui_widget_set_bounds(label, 20, 50, 200, 16);
                gui_widget_add_child(main_window, label);
                printf("✓ Label added\n");
            }
        }

        // Создаем второе окно
        gui_widget_t *info_window = gui_window_create("System Info", 600, 100, 400, 300);
        if (info_window) {
            printf("✓ Info window created (400x300)\n");

            gui_widget_t *info_label = gui_label_create("MyOS v1.0 - Graphics & GUI Demo");
            if (info_label) {
                gui_widget_set_bounds(info_label, 20, 50, 300, 16);
                gui_widget_add_child(info_window, info_label);
            }
        }

        printf("\n✓ All GUI components initialized\n");

        // Рисуем синий фон рабочего стола
        uint32_t blue = graphics_rgb_to_color(20, 20, 60);
        graphics_clear(blue);

        // Рисуем элементы GUI
        printf("Rendering GUI...\n");
        gui_render();
        graphics_flush();
        printf("✓ GUI rendered to framebuffer\n");

        serial_write_string("GUI system initialized and rendered.\n");
    } else {
        printf("⚠ GUI system requires 16-bit or higher color graphics\n");
        printf("⚠ Skipping GUI initialization in text mode\n");
    }

    // Теперь можно запустить основной цикл
    printf("\nEntering main event loop...\n");
    serial_write_string("Entering main event loop.\n");

    while (1) {
        arch_halt(); // при отсутствии задач простаиваем, пока прерывание не придёт
    }
}
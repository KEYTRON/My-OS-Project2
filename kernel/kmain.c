// kmain.c — основная точка входа в C‑код ядра
//
// Ядро MyOS собрано из четырёх языков:
//   ASM  — загрузка и переход в long mode (entry.S), заглушки прерываний
//   C    — драйверы (VGA, serial, клавиатура, таймер), GDT/IDT, этот файл
//   C++  — куча ядра kmalloc/kfree (lib/cxx/kheap.cpp)
//   Rust — интерактивная оболочка (rust/src/lib.rs)
#include <stdint.h>
#include "include/common.h"
#include "include/arch.h"
#include "include/multiboot.h"

// Архитектурно-зависимые заголовки
#ifdef ARCH_X86_64
#include "arch/x86_64/gdt.h"
#include "arch/x86_64/idt.h"
#include "arch/x86_64/paging.h"
#endif

#include "drivers/vga.h"
#include "drivers/serial.h"
#include "drivers/keyboard.h"
#include "drivers/timer.h"
#include "lib/printf.h"
#include "lib/cxx/kheap.h"

// Graphics и GUI
#include "lib/graphics/graphics.h"
#include "lib/gui/gui_widgets.h"
#include "lib/gui/gui_desktop.h"

// Rust-часть ядра
extern void rust_shell_run(void) __attribute__((noreturn));
extern int rust_selftest(void);

static inline void debugcon_write(const char *msg) {
#if defined(__x86_64__) || defined(__i386__)
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
#if defined(__x86_64__) || defined(__i386__)
    __asm__ volatile ("outb %0, %1" : : "a"(code), "Nd"((uint16_t)0xF4));
#else
    (void)code;
#endif
}
#endif

// Функция, вызываемая из entry.S: rdi = multiboot magic, rsi = адрес info
void kernel_main(uint32_t mb_magic, uint32_t mb_info_addr) {
    debugcon_write("[MyOS] kernel_main start\n");

    // Консоль как можно раньше
    vga_init();
    serial_init();

    printf("MyOS kernel booting on ");
#ifdef ARCH_X86_64
    printf("x86_64");
#elif defined(ARCH_ARM64)
    printf("ARM64");
#elif defined(ARCH_RISCV64)
    printf("RISC-V64");
#endif
    printf("...\n");

    // Разбираем multiboot info (до включения чего-либо ещё —
    // структура лежит в низкой памяти, уже закартированной entry.S)
    multiboot_info_t *mbi = 0;
    if (mb_magic == MULTIBOOT_BOOTLOADER_MAGIC && mb_info_addr != 0) {
        mbi = (multiboot_info_t *)(uint64_t)mb_info_addr;
        printf("Multiboot info at 0x%x, flags=0x%x\n", mb_info_addr, mbi->flags);
        if (mbi->flags & MULTIBOOT_INFO_MEMORY) {
            printf("Memory: %u KiB low, %u KiB high\n", mbi->mem_lower, mbi->mem_upper);
        }
#ifdef ARCH_X86_64
        if ((mbi->flags & MULTIBOOT_INFO_FRAMEBUFFER) &&
            mbi->framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
            graphics_set_framebuffer_info(mbi->framebuffer_addr,
                                          mbi->framebuffer_pitch,
                                          mbi->framebuffer_width,
                                          mbi->framebuffer_height,
                                          mbi->framebuffer_bpp);
        }
#endif
    } else {
        printf("WARNING: no multiboot info (magic=0x%x)\n", mb_magic);
    }

#ifdef ARCH_X86_64
    gdt_init();
    printf("GDT initialized.\n");

    idt_init();
    printf("IDT initialized (256 vectors, PIC remapped).\n");

    paging_init();
#endif

    // Куча ядра (аллокатор написан на C++)
    kheap_init();
    {
        uint64_t total = 0;
        kheap_stats(&total, 0, 0);
        printf("Kernel heap initialized: %lu KiB (C++ allocator).\n", total / 1024);
    }

    // Таймер и клавиатура
    timer_init();
    printf("PIT timer initialized at %d Hz.\n", TIMER_HZ);

    keyboard_init();
    printf("Keyboard driver initialized.\n");

    // Включаем прерывания
    arch_enable_interrupts();
    printf("Interrupts enabled.\n");

    // Самопроверка Rust-части (аллокации через C++ кучу)
    if (rust_selftest() == 0) {
        printf("Rust runtime selftest: OK\n");
    } else {
        printf("Rust runtime selftest: FAILED\n");
    }

    printf("\n");
    printf("========================================================\n");
    printf("            Welcome to MyOS v1.0\n");
    printf("  ASM + C + C++ + Rust hobby operating system\n");
    printf("========================================================\n\n");

    serial_write_string("Kernel init complete.\n");

#ifdef ENABLE_QEMU_EXIT
    // CI smoke-тест: выходим только ПОСЛЕ полной инициализации,
    // чтобы падение на любом этапе валило тест.
    debugcon_write("[MyOS] init complete, signalling qemu exit\n");
    qemu_exit(0);
#endif

    // Графика: если GRUB дал framebuffer — рисуем рабочий стол
    graphics_device_t *gfx = graphics_init();
    if (gfx != NULL && gfx->mode == GRAPHICS_MODE_VESA && gfx->bpp >= 16) {
        printf("Graphics: %ux%u @ %ubpp\n", gfx->width, gfx->height, gfx->bpp);
        gui_init();
        gui_desktop_run(gfx);
        printf("GUI desktop rendered. Shell continues on serial console.\n");
    } else {
        printf("Running in VGA text mode.\n");
    }

    // Интерактивная оболочка (Rust). Не возвращается.
    rust_shell_run();
}

#include "interrupts.h"
#include "keyboard.h"
#include "paging.h"
#include "vga.h"
#include "fs.h"
#include "shell.h"
#include "task.h"
#include "ata.h"
#include "elf.h"

static void demo_task1() {
    print_string("Task1 running\n");
}

static void demo_task2() {
    print_string("Task2 running\n");
}

void kernel_main(void) {
    clear_screen();
    init_interrupts();
    init_paging();
    fs_init();
    ata_init();
    print_string("Welcome to MyOS!\n");
    print_string("Kernel initialized.\n");
    init_keyboard();

    tasking_init();
    task_create(demo_task1);
    task_create(demo_task2);
    print_string("Starting simple multitasking demo...\n");
    for (int i = 0; i < 5; i++) {
        task_run_once();
    }

    unsigned char sector[512];
    ata_read_sector(0, sector);
    print_string("First bytes of disk: \n");
    for (int i = 0; i < 16; i++) {
        char c = sector[i];
        char out[3] = {
            "0123456789ABCDEF"[(c>>4)&0xF],
            "0123456789ABCDEF"[c&0xF],
            "0123456789ABCDEF"[(c >> 4) & 0xF],
            "0123456789ABCDEF"[c & 0xF],
            '\0'
        };
        print_string(out);
        put_char(' ');
    }
    print_string("\n");

    void (*entry)();
    if (elf_load((const char*)sector, 512, &entry) == 0) {
        print_string("ELF entry: ");
        char buf[9];
        unsigned int addr = (unsigned int)entry;
        for (int i = 7; i >= 0; i--) {
            buf[i] = "0123456789ABCDEF"[addr & 0xF];
            addr >>= 4;
        }
        buf[8] = '\0';
        print_string(buf);
        print_string("\n");
    }

    print_string("Starting shell...\n");
    shell();
    print_string("Starting shell...\n");
    shell();

    while (1) {}
}

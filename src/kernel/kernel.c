#include "interrupts.h"
#include "keyboard.h"
#include "paging.h"
#include "vga.h"
#include "fs.h"
#include "shell.h"

void kernel_main(void) {
    clear_screen();
    init_interrupts();
    init_paging();
    fs_init();
    print_string("Welcome to MyOS!\n");
    print_string("Kernel initialized.\n");
    init_keyboard();
    print_string("Starting shell...\n");
    shell();
    while (1) {}
}

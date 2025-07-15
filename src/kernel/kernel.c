#include "interrupts.h"
#include "keyboard.h"
#include "paging.h"
#include "vga.h"

void kernel_main(void) {
    clear_screen();
    init_interrupts();
    init_paging();
    print_string("Welcome to MyOS!\n");
    print_string("Kernel initialized.\n");
    init_keyboard();

}


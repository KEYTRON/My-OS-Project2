#include "interrupts.h"
#include "keyboard.h"
#include "paging.h"
#include "vga.h"
#include "fs.h"

void kernel_main(void) {
    clear_screen();
    init_interrupts();
    init_paging();
    fs_init();
    print_string("Welcome to MyOS!\n");
    print_string("Kernel initialized.\n");
    init_keyboard();
    print_string("Type something and press Enter:\n");
    char buf[128];
    read_line(buf, sizeof(buf));
    print_string("\nYou typed: ");
    print_string(buf);
    print_string("\nSaving to file...\n");
    fs_create("input.txt", buf);
    print_string("Files:\n");
    fs_list();
    print_string("Drawing...\n");
    clear_graphics(0);
    for (int y = 0; y < 200; y++) {
        for (int x = 0; x < 320; x++) {
            if ((x + y) % 20 == 0) {
                put_pixel(x, y, 12); // red
            }
        }
    }
    while (1) {}
}


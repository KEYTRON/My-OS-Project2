#include "interrupts.h"
#include "keyboard.h"
#include "paging.h"
#include "vga.h"
#include "fs.h"
#include "shell.h"
#include "task.h"

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

    print_string("Starting shell...\n");
    shell();
    while (1) {}
}

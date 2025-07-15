#include "task.h"
#include "vga.h"

#define MAX_TASKS 4

typedef void (*task_func)();

static task_func tasks[MAX_TASKS];
static int task_count = 0;

void tasking_init() {
    task_count = 0;
}

void task_create(task_func func) {
    if (task_count < MAX_TASKS) {
        tasks[task_count++] = func;
    }
}

void task_run_once() {
    for (int i = 0; i < task_count; i++) {
        tasks[i]();
    }
}

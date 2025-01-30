// process.c
#include "process.h"
#include "memory.h"
#include "scheduler/scheduler.h"

#define MAX_PROCESSES 128

static int process_count = 0;
static struct process processes[MAX_PROCESSES];

void create_process(void (*entry_point)()) {
    if (process_count >= MAX_PROCESSES) {
        return;  // Превышено количество процессов
    }
    
    struct process *proc = &processes[process_count];
    proc->id = process_count;
    proc->entry_point = entry_point;
    proc->page_dir = (uint32_t *)alloc_page();

    process_count++;
    scheduler();  // Запуск планировщика
}

void kill_process(int pid) {
    if (pid < 0 || pid >= process_count) {
        return;
    }

    free_page(processes[pid].page_dir);
    processes[pid] = processes[process_count - 1];  // Перемещаем последний процесс на место удаляемого
    process_count--;
}
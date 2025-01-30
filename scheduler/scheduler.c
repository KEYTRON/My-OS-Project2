// scheduler.c
#include "scheduler.h"
#include "../interrupts/isr.h"

// Список процессов
#define MAX_PROCESSES 128
static int current_process = 0;
static int process_count = 0;

void scheduler() {
    current_process = (current_process + 1) % process_count;
    load_process(current_process);
}

void load_process(int process_id) {
    // Загружаем процесс с данным ID
    // В будущем тут будет работать код переключения контекста
}
// memory.c
#include "memory.h"

static uint32_t *memory_map = NULL;

void init_memory(uint32_t *map) {
    memory_map = map;
}

void *alloc_page() {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory_map[i] == 0) {  // Свободная страница
            memory_map[i] = 1;  // Отмечаем страницу как занятую
            return (void *)(i * PAGE_SIZE);  // Возвращаем адрес страницы
        }
    }
    return NULL;  // Нет свободных страниц
}

void free_page(void *ptr) {
    uint32_t index = (uint32_t)ptr / PAGE_SIZE;
    if (index < MEMORY_SIZE) {
        memory_map[index] = 0;  // Освобождаем страницу
    }
}
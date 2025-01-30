// memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define PAGE_SIZE 4096
#define MEMORY_SIZE 1024  // Количество страниц

void init_memory(uint32_t *map);
void *alloc_page();
void free_page(void *ptr);

#endif
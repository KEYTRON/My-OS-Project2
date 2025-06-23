#include "memory.h"

// Начало свободной памяти после ядра (просто примерный адрес)
#define MEMORY_START 0x100000
static unsigned int next_free = MEMORY_START;

void* malloc_simple(unsigned int size) {
    void* ptr = (void*)next_free;
    next_free += size;
    return ptr;
}

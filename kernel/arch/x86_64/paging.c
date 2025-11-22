// paging.c - x86_64 paging initialization status
#include <stdint.h>
#include "paging.h"
#include "../../lib/printf.h"

static inline uint64_t read_cr3(void) {
    uint64_t value;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(value));
    return value;
}

void paging_init() {
    uint64_t cr3 = read_cr3();
    printf("Paging enabled; CR3=0x%016lx (set up in entry.S)\n", cr3);
}

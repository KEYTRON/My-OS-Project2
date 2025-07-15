#include "paging.h"

static unsigned int __attribute__((aligned(4096))) page_directory[1024];
static unsigned int __attribute__((aligned(4096))) first_page_table[1024];

void init_paging() {
    for (int i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 0x1000) | 3; // Present, RW
    }
    page_directory[0] = ((unsigned int)first_page_table) | 3;
    for (int i = 1; i < 1024; i++) {
        page_directory[i] = 0;
    }

    // Load page directory into CR3
    __asm__ volatile("mov %0, %%cr3" :: "r"(page_directory));

    // Enable paging
    unsigned int cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
}

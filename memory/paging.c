// paging.c
#include "paging.h"
#include "memory.h"

static uint32_t *page_directory = NULL;

void init_paging() {
    page_directory = (uint32_t *)alloc_page();
    // Инициализация страниц и таблиц
}

void map_page(uint32_t *page_table, uint32_t phys_addr, uint32_t virt_addr) {
    page_table[virt_addr / PAGE_SIZE] = phys_addr | 0x3;  // Страница доступна для чтения и записи
}
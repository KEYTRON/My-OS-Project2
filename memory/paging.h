// paging.h
#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

void init_paging();
void map_page(uint32_t *page_table, uint32_t phys_addr, uint32_t virt_addr);

#endif
// paging.c — минимальная настройка пейджинга (включим 1:1 отображение)
// + скелет для дальнейшего развития

#include <stdint.h>
#include "paging.h"
#include "../lib/mem.h"
#include "../lib/printf.h"

// Адрес начала физической памяти (обычно 0x100000, т. е. после 1 MB)
#define PHYS_MEMORY_OFFSET 0x100000

// Определяем таблицы (выравненные на 4 KB)
static uint64_t pml4_table[512] __attribute__((aligned(4096)));
static uint64_t pdpt_table[512] __attribute__((aligned(4096)));
static uint64_t pd_table[512]   __attribute__((aligned(4096)));
// мы не используем 2‑уровневые таблицы (Page Directories на 4 MB), а ограничимся 1:1

// Скелет функции инициализации пейджинга
void paging_init() {
    // 1) Обнулим все таблицы
    memset(pml4_table, 0, sizeof(pml4_table));
    memset(pdpt_table, 0, sizeof(pdpt_table));
    memset(pd_table,   0, sizeof(pd_table));

    // 2) Заполним pd_table так, чтобы первые 2 MB отображались (или всё, что нужно)
    for (int i = 0; i < 512; i++) {
        // Каждая запись PD (Page Directory) указывает на 2 MB страницу (PS bit = 1)
        // Адрес = (i * 2MB)
        pd_table[i] = (i * 0x200000) | 0x83; // present=1, rw=1, user=0, PS=1
    }

    // 3) Укажем в PDPT на PD
    pdpt_table[0] = ((uint64_t)pd_table) | 0x03; // present=1, rw=1

    // 4) Укажем в PML4 на PDPT
    pml4_table[0] = ((uint64_t)pdpt_table) | 0x03;

    // 5) Сохраним адрес PML4 в CR3
    asm volatile("mov cr3, %0" : : "r"(&pml4_table));

    // 6) Включим в CR0 бит PG (Paging)
    uint64_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 1 << 31;  // PG = 1
    asm volatile("mov %0, %%cr0" :: "r"(cr0));

    printf("Paging enabled.\n");
}
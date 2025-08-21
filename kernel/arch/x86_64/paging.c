// paging.c — минимальная настройка пейджинга (включим 1:1 отображение)
// + скелет для дальнейшего развития

#include <stdint.h>
#include "paging.h"
#include "../../lib/string.h"
#include "../../lib/printf.h"

// Адрес начала физической памяти (обычно 0x100000, т. е. после 1 MB)
#define PHYS_MEMORY_OFFSET 0x100000

// Определяем таблицы (выравненные на 4KB)
static uint64_t pml4_table[512] __attribute__((aligned(4096))) __attribute__((unused));
static uint64_t pdpt_table[512] __attribute__((aligned(4096))) __attribute__((unused));
static uint64_t pd_table[512]   __attribute__((aligned(4096))) __attribute__((unused));
// мы не используем 2-уровневые таблицы (Page Directories на 4 MB), а ограничимся 1:1

// Скелет функции инициализации пейджинга
void paging_init() {
    // Пейджинг уже настроен в entry.S, поэтому просто выводим сообщение
    printf("Paging already enabled in entry.S.\n");
}
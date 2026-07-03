// kheap.h — C-интерфейс кучи ядра (реализация на C++ в kheap.cpp)
#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Инициализация кучи (вызывается один раз из kmain)
void kheap_init(void);

// Классические kmalloc/kfree. kmalloc возвращает NULL при нехватке памяти.
void* kmalloc(size_t size);
void* kmalloc_aligned(size_t size, size_t align);
void  kfree(void* ptr);
void* krealloc(void* ptr, size_t new_size);

// Статистика: total — размер кучи, used — занято (включая заголовки),
// free — свободно. Любой из указателей может быть NULL.
void kheap_stats(uint64_t* total, uint64_t* used, uint64_t* free_bytes);

#ifdef __cplusplus
}
#endif

#endif // KHEAP_H

// kheap.cpp — куча ядра: free-list аллокатор first-fit со слиянием
// соседних свободных блоков. Написано на freestanding C++ (без исключений
// и RTTI); наружу выставлен C-интерфейс из kheap.h.
#include "kheap.h"

namespace {

constexpr size_t kHeapSize = 16 * 1024 * 1024; // 16 MiB
constexpr size_t kMinSplit = 32;               // не плодим блоки мельче
constexpr uint32_t kMagicFree = 0xF7EEB10C;
constexpr uint32_t kMagicUsed = 0xA110CA7E;

struct BlockHeader {
    uint32_t magic;
    uint32_t reserved;
    size_t size;          // размер полезной области (без заголовка)
    BlockHeader* next;    // следующий блок по адресам
    BlockHeader* prev;    // предыдущий блок по адресам

    bool is_free() const { return magic == kMagicFree; }
    void mark_free() { magic = kMagicFree; }
    void mark_used() { magic = kMagicUsed; }
    uint8_t* payload() { return reinterpret_cast<uint8_t*>(this + 1); }
};

class Heap {
public:
    void init(uint8_t* base, size_t size) {
        base_ = base;
        total_ = size;
        auto* first = reinterpret_cast<BlockHeader*>(base);
        first->size = size - sizeof(BlockHeader);
        first->next = nullptr;
        first->prev = nullptr;
        first->mark_free();
        head_ = first;
        initialized_ = true;
    }

    bool ready() const { return initialized_; }

    void* alloc(size_t size, size_t align) {
        if (!initialized_ || size == 0) return nullptr;
        size = align_up(size, 16);

        for (BlockHeader* b = head_; b; b = b->next) {
            if (!b->is_free()) continue;

            uintptr_t payload = reinterpret_cast<uintptr_t>(b->payload());
            uintptr_t aligned = align_up(payload, align);
            size_t padding = aligned - payload;

            if (b->size < size + padding) continue;

            // При необходимости отрезаем паддинг в отдельный свободный блок
            if (padding != 0) {
                if (padding < sizeof(BlockHeader) + kMinSplit) {
                    // слишком маленький зазор — ищем блок дальше
                    // (простая стратегия, зато без коррупции)
                    continue;
                }
                b = split(b, padding - sizeof(BlockHeader));
                if (!b) continue;
            }

            // Отрезаем хвост, если остаток достаточно велик
            if (b->size >= size + sizeof(BlockHeader) + kMinSplit) {
                split(b, size);
            }

            b->mark_used();
            return b->payload();
        }
        return nullptr;
    }

    void free(void* ptr) {
        if (!ptr || !initialized_) return;
        auto* b = reinterpret_cast<BlockHeader*>(ptr) - 1;
        if (b->magic != kMagicUsed) return; // двойное освобождение/мусор
        b->mark_free();
        coalesce(b);
    }

    size_t usable_size(void* ptr) const {
        if (!ptr) return 0;
        auto* b = reinterpret_cast<const BlockHeader*>(ptr) - 1;
        return (b->magic == kMagicUsed) ? b->size : 0;
    }

    void stats(uint64_t* total, uint64_t* used, uint64_t* free_bytes) const {
        uint64_t f = 0;
        for (BlockHeader* b = head_; b; b = b->next) {
            if (b->is_free()) f += b->size;
        }
        if (total) *total = total_;
        if (free_bytes) *free_bytes = f;
        if (used) *used = total_ - f;
    }

private:
    static uintptr_t align_up(uintptr_t v, size_t a) {
        return (v + a - 1) & ~(uintptr_t)(a - 1);
    }

    // Разрезает блок b: первые keep байт остаются в b, остаток становится
    // новым свободным блоком. Возвращает новый блок (хвост).
    BlockHeader* split(BlockHeader* b, size_t keep) {
        if (b->size < keep + sizeof(BlockHeader) + kMinSplit) return nullptr;
        auto* tail = reinterpret_cast<BlockHeader*>(b->payload() + keep);
        tail->size = b->size - keep - sizeof(BlockHeader);
        tail->next = b->next;
        tail->prev = b;
        tail->mark_free();
        if (b->next) b->next->prev = tail;
        b->next = tail;
        b->size = keep;
        return tail;
    }

    // Сливает блок со свободными соседями
    void coalesce(BlockHeader* b) {
        if (b->next && b->next->is_free()) {
            b->size += sizeof(BlockHeader) + b->next->size;
            b->next = b->next->next;
            if (b->next) b->next->prev = b;
        }
        if (b->prev && b->prev->is_free()) {
            b->prev->size += sizeof(BlockHeader) + b->size;
            b->prev->next = b->next;
            if (b->next) b->next->prev = b->prev;
        }
    }

    uint8_t* base_ = nullptr;
    size_t total_ = 0;
    BlockHeader* head_ = nullptr;
    bool initialized_ = false;
};

Heap g_heap;

// Хранилище кучи размещаем в .bss — оно гарантированно попадает в память,
// закартированную bootstrap-таблицами страниц из entry.S.
alignas(4096) uint8_t g_heap_storage[kHeapSize];

} // namespace

extern "C" {

void kheap_init(void) {
    g_heap.init(g_heap_storage, kHeapSize);
}

void* kmalloc(size_t size) {
    return g_heap.alloc(size, 16);
}

void* kmalloc_aligned(size_t size, size_t align) {
    if (align < 16) align = 16;
    return g_heap.alloc(size, align);
}

void kfree(void* ptr) {
    g_heap.free(ptr);
}

void* krealloc(void* ptr, size_t new_size) {
    if (!ptr) return kmalloc(new_size);
    if (new_size == 0) {
        kfree(ptr);
        return nullptr;
    }
    size_t old = g_heap.usable_size(ptr);
    if (old >= new_size) return ptr;

    void* fresh = kmalloc(new_size);
    if (!fresh) return nullptr;
    auto* dst = reinterpret_cast<uint8_t*>(fresh);
    auto* src = reinterpret_cast<uint8_t*>(ptr);
    for (size_t i = 0; i < old; i++) dst[i] = src[i];
    kfree(ptr);
    return fresh;
}

void kheap_stats(uint64_t* total, uint64_t* used, uint64_t* free_bytes) {
    g_heap.stats(total, used, free_bytes);
}

} // extern "C"

// ---- Глобальные операторы new/delete поверх кучи ядра ----

void* operator new(size_t size) { return kmalloc(size); }
void* operator new[](size_t size) { return kmalloc(size); }
void operator delete(void* p) noexcept { kfree(p); }
void operator delete[](void* p) noexcept { kfree(p); }
void operator delete(void* p, size_t) noexcept { kfree(p); }
void operator delete[](void* p, size_t) noexcept { kfree(p); }

#ifndef ARCH_H
#define ARCH_H

// Определяем архитектуру на этапе компиляции
#if defined(__x86_64__) || defined(__amd64__) || (defined(__i386__) && !defined(__INTEL_COMPILER__))
    #define ARCH_X86_64
    #include "../arch/x86_64/arch.h"
    typedef x86_64_reg_t arch_reg_t;
    typedef x86_64_inst_t arch_inst_t;
#elif defined(__aarch64__) || defined(__arm64__)
    #define ARCH_ARM64
    #include "../arch/arm64/arch.h"
    typedef arm64_reg_t arch_reg_t;
    typedef arm64_inst_t arch_inst_t;
#elif defined(__riscv) && __riscv_xlen == 64
    #define ARCH_RISCV64
    #include "../arch/riscv64/arch.h"
    typedef riscv64_reg_t arch_reg_t;
    typedef riscv64_inst_t arch_inst_t;
#else
    #error "Unsupported architecture"
#endif

// Архитектурно-независимые типы
typedef uint64_t arch_addr_t;
typedef uint64_t arch_size_t;

// Архитектурно-независимые константы
#define ARCH_PAGE_SIZE 4096
#define ARCH_PAGE_SHIFT 12

// Архитектурно-независимые функции
static inline void arch_disable_interrupts(void) {
#ifdef ARCH_X86_64
    x86_64_cli();
#elif defined(ARCH_ARM64)
    arm64_disable_irq();
#elif defined(ARCH_RISCV64)
    riscv64_disable_interrupts();
#endif
}

static inline void arch_enable_interrupts(void) {
#ifdef ARCH_X86_64
    x86_64_sti();
#elif defined(ARCH_ARM64)
    arm64_enable_irq();
#elif defined(ARCH_RISCV64)
    riscv64_enable_interrupts();
#endif
}

static inline void arch_halt(void) {
#ifdef ARCH_X86_64
    x86_64_hlt();
#elif defined(ARCH_ARM64)
    asm volatile("wfe");
#elif defined(ARCH_RISCV64)
    asm volatile("wfi");
#endif
}

static inline void arch_invalidate_tlb(void) {
#ifdef ARCH_X86_64
    x86_64_invalidate_tlb();
#elif defined(ARCH_ARM64)
    arm64_invalidate_tlb();
#elif defined(ARCH_RISCV64)
    riscv64_sfence_vma();
#endif
}

static inline void arch_invalidate_icache(void) {
#ifdef ARCH_X86_64
    x86_64_invalidate_icache();
#elif defined(ARCH_ARM64)
    arm64_invalidate_icache();
#elif defined(ARCH_RISCV64)
    riscv64_fence_i();
#endif
}

// Архитектурно-независимые макросы для работы с памятью
#define ARCH_ALIGN_UP(addr, align) (((addr) + (align) - 1) & ~((align) - 1))
#define ARCH_ALIGN_DOWN(addr, align) ((addr) & ~((align) - 1))
#define ARCH_IS_ALIGNED(addr, align) (((addr) & ((align) - 1)) == 0)

// Архитектурно-независимые функции для работы с памятью
static inline void arch_memory_barrier(void) {
#ifdef ARCH_X86_64
    asm volatile("mfence" : : : "memory");
#elif defined(ARCH_ARM64)
    asm volatile("dmb ish" : : : "memory");
#elif defined(ARCH_RISCV64)
    asm volatile("fence rw,rw" : : : "memory");
#endif
}

static inline void arch_read_barrier(void) {
#ifdef ARCH_X86_64
    asm volatile("lfence" : : : "memory");
#elif defined(ARCH_ARM64)
    asm volatile("dmb ish" : : : "memory");
#elif defined(ARCH_RISCV64)
    asm volatile("fence r,r" : : : "memory");
#endif
}

static inline void arch_write_barrier(void) {
#ifdef ARCH_X86_64
    asm volatile("sfence" : : : "memory");
#elif defined(ARCH_ARM64)
    asm volatile("dmb ish" : : : "memory");
#elif defined(ARCH_RISCV64)
    asm volatile("fence w,w" : : : "memory");
#endif
}

// Архитектурно-независимые функции для работы с атомарными операциями
static inline uint32_t arch_atomic_add(volatile uint32_t* ptr, uint32_t value) {
#ifdef ARCH_X86_64
    uint32_t result;
    asm volatile("lock xaddl %0, %1" : "=r"(result), "+m"(*ptr) : "0"(value) : "memory");
    return result;
#elif defined(ARCH_ARM64)
    uint32_t result;
    asm volatile("ldadd %0, %1, [%2]" : "=r"(result) : "r"(value), "r"(ptr) : "memory");
    return result;
#elif defined(ARCH_RISCV64)
    uint32_t result;
    asm volatile("amoadd.w %0, %1, (%2)" : "=r"(result) : "r"(value), "r"(ptr) : "memory");
    return result;
#endif
}

static inline uint32_t arch_atomic_compare_exchange(volatile uint32_t* ptr, uint32_t expected, uint32_t desired) {
#ifdef ARCH_X86_64
    uint32_t result;
    asm volatile("lock cmpxchgl %2, %1" : "=a"(result), "+m"(*ptr) : "r"(desired), "0"(expected) : "memory");
    return result;
#elif defined(ARCH_ARM64)
    uint32_t result;
    asm volatile("cas %0, %1, [%2]" : "=r"(result) : "r"(expected), "r"(ptr) : "memory");
    return result;
#elif defined(ARCH_RISCV64)
    uint32_t result;
    asm volatile("amoswap.w %0, %1, (%2)" : "=r"(result) : "r"(desired), "r"(ptr) : "memory");
    return result;
#endif
}

#endif // ARCH_H

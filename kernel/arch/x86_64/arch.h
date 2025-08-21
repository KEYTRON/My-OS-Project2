#ifndef ARCH_X86_64_H
#define ARCH_X86_64_H

#include <stdint.h>

// x86_64 специфичные типы
typedef uint64_t x86_64_reg_t;
typedef uint32_t x86_64_inst_t;

// Системные регистры
#define X86_64_CR0 0
#define X86_64_CR1 1
#define X86_64_CR2 2
#define X86_64_CR3 3
#define X86_64_CR4 4

// Флаги CR0
#define X86_64_CR0_PE (1 << 0)   // Protected Mode Enable
#define X86_64_CR0_MP (1 << 1)   // Monitor Coprocessor
#define X86_64_CR0_EM (1 << 2)   // Emulation
#define X86_64_CR0_TS (1 << 3)   // Task Switched
#define X86_64_CR0_ET (1 << 4)   // Extension Type
#define X86_64_CR0_NE (1 << 5)   // Numeric Error
#define X86_64_CR0_WP (1 << 16)  // Write Protect
#define X86_64_CR0_AM (1 << 18)  // Alignment Mask
#define X86_64_CR0_NW (1 << 29)  // Not Write-through
#define X86_64_CR0_CD (1 << 30)  // Cache Disable
#define X86_64_CR0_PG (1 << 31)  // Paging

// Флаги CR4
#define X86_64_CR4_VME (1 << 0)      // Virtual-8086 Mode Extensions
#define X86_64_CR4_PVI (1 << 1)      // Protected-Mode Virtual Interrupts
#define X86_64_CR4_TSD (1 << 2)      // Time Stamp Disable
#define X86_64_CR4_DE  (1 << 3)      // Debugging Extensions
#define X86_64_CR4_PSE (1 << 4)      // Page Size Extensions
#define X86_64_CR4_PAE (1 << 5)      // Physical Address Extension
#define X86_64_CR4_MCE (1 << 6)      // Machine Check Enable
#define X86_64_CR4_PGE (1 << 7)      // Page Global Enable
#define X86_64_CR4_PCE (1 << 8)      // Performance-Monitoring Counter Enable
#define X86_64_CR4_OSFXSR (1 << 9)   // OS Support for FXSAVE/FXRSTOR
#define X86_64_CR4_OSXMMEXCPT (1 << 10) // OS Support for Unmasked SIMD FP Exceptions
#define X86_64_CR4_LA57 (1 << 12)    // Linear Address 57-bit
#define X86_64_CR4_VMXE (1 << 13)    // VMX Enable
#define X86_64_CR4_SMXE (1 << 14)    // SMX Enable
#define X86_64_CR4_FSGSBASE (1 << 16) // FSGSBASE Enable
#define X86_64_CR4_PCIDE (1 << 17)   // PCID Enable
#define X86_64_CR4_OSXSAVE (1 << 18) // XSAVE and Processor Extended States Enable
#define X86_64_CR4_SMEP (1 << 20)    // SMEP Enable
#define X86_64_CR4_SMAP (1 << 21)    // SMAP Enable
#define X86_64_CR4_PKE (1 << 22)     // Protection Key Enable

// Функции для работы с системными регистрами
static inline x86_64_reg_t x86_64_read_cr(uint32_t cr) {
    x86_64_reg_t val;
    switch(cr) {
        case 0: asm volatile("mov %%cr0, %0" : "=r"(val)); break;
        case 2: asm volatile("mov %%cr2, %0" : "=r"(val)); break;
        case 3: asm volatile("mov %%cr3, %0" : "=r"(val)); break;
        case 4: asm volatile("mov %%cr4, %0" : "=r"(val)); break;
        default: val = 0; break;
    }
    return val;
}

static inline void x86_64_write_cr(uint32_t cr, x86_64_reg_t val) {
    switch(cr) {
        case 0: asm volatile("mov %0, %%cr0" : : "r"(val)); break;
        case 3: asm volatile("mov %0, %%cr3" : : "r"(val)); break;
        case 4: asm volatile("mov %0, %%cr4" : : "r"(val)); break;
    }
}

// Функции для работы с прерываниями
static inline void x86_64_cli(void) {
    asm volatile("cli");
}

static inline void x86_64_sti(void) {
    asm volatile("sti");
}

static inline void x86_64_hlt(void) {
    asm volatile("hlt");
}

// Функции для работы с памятью
static inline void x86_64_invalidate_tlb(void) {
    asm volatile("invlpg (%0)" : : "r" (0) : "memory");
}

static inline void x86_64_invalidate_icache(void) {
    asm volatile("wbinvd");
}

// Функции для работы с кэшем
static inline void x86_64_wbinvd(void) {
    asm volatile("wbinvd");
}

static inline void x86_64_clflush(void* addr) {
    asm volatile("clflush (%0)" : : "r" (addr) : "memory");
}

// Функции для работы с MSR
static inline x86_64_reg_t x86_64_read_msr(uint32_t msr) {
    uint32_t low, high;
    asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((x86_64_reg_t)high << 32) | low;
}

static inline void x86_64_write_msr(uint32_t msr, x86_64_reg_t value) {
    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = (value >> 32) & 0xFFFFFFFF;
    asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(msr));
}

// Функции для работы с портами ввода-вывода
static inline uint8_t x86_64_inb(uint16_t port) {
    uint8_t val;
    asm volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static inline void x86_64_outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t x86_64_inw(uint16_t port) {
    uint16_t val;
    asm volatile("inw %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static inline void x86_64_outw(uint16_t port, uint16_t val) {
    asm volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint32_t x86_64_inl(uint16_t port) {
    uint32_t val;
    asm volatile("inl %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static inline void x86_64_outl(uint16_t port, uint32_t val) {
    asm volatile("outl %0, %1" : : "a"(val), "Nd"(port));
}

#endif // ARCH_X86_64_H

#ifndef ARCH_ARM64_H
#define ARCH_ARM64_H

#include <stdint.h>

// ARM64 специфичные типы
typedef uint64_t arm64_reg_t;
typedef uint32_t arm64_inst_t;

// Системные регистры
#define ARM64_MPIDR_EL1     "mpidr_el1"
#define ARM64_ELR_EL1       "elr_el1"
#define ARM64_ESR_EL1       "esr_el1"
#define ARM64_FAR_EL1       "far_el1"
#define ARM64_SPSR_EL1      "spsr_el1"
#define ARM64_CNTP_CTL_EL0  "cntp_ctl_el0"
#define ARM64_CNTP_TVAL_EL0 "cntp_tval_el0"
#define ARM64_CNTP_CVAL_EL0 "cntp_cval_el0"

// Уровни исключений
#define ARM64_EL0 0
#define ARM64_EL1 1
#define ARM64_EL2 2
#define ARM64_EL3 3

// Флаги прерываний
#define ARM64_DAIF_D  (1 << 9)
#define ARM64_DAIF_A  (1 << 8)
#define ARM64_DAIF_I  (1 << 7)
#define ARM64_DAIF_F  (1 << 6)

// Функции для работы с системными регистрами
static inline arm64_reg_t arm64_read_sysreg(const char* reg) {
    arm64_reg_t val;
    asm volatile("mrs %0, " reg : "=r"(val));
    return val;
}

static inline void arm64_write_sysreg(const char* reg, arm64_reg_t val) {
    asm volatile("msr " reg ", %0" : : "r"(val));
}

// Функции для работы с прерываниями
static inline void arm64_disable_irq(void) {
    asm volatile("msr daifset, #2");
}

static inline void arm64_enable_irq(void) {
    asm volatile("msr daifclr, #2");
}

static inline void arm64_disable_fiq(void) {
    asm volatile("msr daifset, #1");
}

static inline void arm64_enable_fiq(void) {
    asm volatile("msr daifclr, #1");
}

// Функции для работы с памятью
static inline void arm64_invalidate_tlb(void) {
    asm volatile("tlbi vmalle1");
    asm volatile("dsb ish");
    asm volatile("isb");
}

static inline void arm64_invalidate_icache(void) {
    asm volatile("ic ialluis");
    asm volatile("dsb ish");
    asm volatile("isb");
}

// Функции для работы с кэшем
static inline void arm64_clean_dcache(void) {
    asm volatile("dsb ish");
}

static inline void arm64_clean_invalidate_dcache(void) {
    asm volatile("dsb ish");
    asm volatile("isb");
}

#endif // ARCH_ARM64_H

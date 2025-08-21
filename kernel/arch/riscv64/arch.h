#ifndef ARCH_RISCV64_H
#define ARCH_RISCV64_H

#include <stdint.h>

// RISC-V64 специфичные типы
typedef uint64_t riscv64_reg_t;
typedef uint32_t riscv64_inst_t;

// Системные регистры
#define RISCV64_MSTATUS     "mstatus"
#define RISCV64_MIE         "mie"
#define RISCV64_MIP         "mip"
#define RISCV64_MTVEC       "mtvec"
#define RISCV64_MEPC        "mepc"
#define RISCV64_MCAUSE      "mcause"
#define RISCV64_MTVAL       "mtval"
#define RISCV64_MHARTID     "mhartid"
#define RISCV64_MSCRATCH    "mscratch"

// Привилегированные уровни
#define RISCV64_MODE_U 0
#define RISCV64_MODE_S 1
#define RISCV64_MODE_M 3

// Флаги прерываний
#define RISCV64_MIP_MSIP (1 << 3)  // Machine software interrupt
#define RISCV64_MIP_MTIP (1 << 7)  // Machine timer interrupt
#define RISCV64_MIP_MEIP (1 << 11) // Machine external interrupt

// Функции для работы с системными регистрами
static inline riscv64_reg_t riscv64_read_csr(const char* csr) {
    riscv64_reg_t val;
    asm volatile("csrr %0, " csr : "=r"(val));
    return val;
}

static inline void riscv64_write_csr(const char* csr, riscv64_reg_t val) {
    asm volatile("csrw " csr ", %0" : : "r"(val));
}

static inline riscv64_reg_t riscv64_set_csr(const char* csr, riscv64_reg_t val) {
    riscv64_reg_t old;
    asm volatile("csrrs %0, " csr ", %1" : "=r"(old) : "r"(val));
    return old;
}

static inline riscv64_reg_t riscv64_clear_csr(const char* csr, riscv64_reg_t val) {
    riscv64_reg_t old;
    asm volatile("csrrc %0, " csr ", %1" : "=r"(old) : "r"(val));
    return old;
}

// Функции для работы с прерываниями
static inline void riscv64_enable_interrupts(void) {
    asm volatile("csrsi mstatus, 0x8");
}

static inline void riscv64_disable_interrupts(void) {
    asm volatile("csrci mstatus, 0x8");
}

static inline void riscv64_enable_timer_interrupt(void) {
    riscv64_set_csr("mie", RISCV64_MIP_MTIP);
}

static inline void riscv64_disable_timer_interrupt(void) {
    riscv64_clear_csr("mie", RISCV64_MIP_MTIP);
}

// Функции для работы с памятью
static inline void riscv64_sfence_vma(void) {
    asm volatile("sfence.vma");
}

static inline void riscv64_fence_i(void) {
    asm volatile("fence.i");
}

static inline void riscv64_fence_rw_rw(void) {
    asm volatile("fence rw,rw");
}

// Функции для работы с кэшем
static inline void riscv64_clean_dcache(void) {
    // RISC-V не имеет встроенных инструкций для работы с кэшем
    // Это должно быть реализовано через platform-specific код
    riscv64_fence_rw_rw();
}

static inline void riscv64_invalidate_icache(void) {
    riscv64_fence_i();
}

// Функции для работы с таймером
static inline void riscv64_set_timer(riscv64_reg_t value) {
    asm volatile("csrw mtimecmp, %0" : : "r"(value));
}

static inline riscv64_reg_t riscv64_read_timer(void) {
    riscv64_reg_t value;
    asm volatile("csrr %0, mtime" : "=r"(value));
    return value;
}

#endif // ARCH_RISCV64_H

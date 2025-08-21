// entry.S - точка входа для RISC-V64 архитектуры
// Код написан для ассемблера GNU Assembler (GAS)

.section .text.boot
.global _start

_start:
    // Отключаем прерывания
    csrw sie, zero
    csrw sip, zero
    
    // Получаем ID процессора
    csrr t0, mhartid
    bnez t0, 1f
    
    // Инициализируем стек для CPU0
    la sp, _start
    addi sp, sp, 0x1000
    
    // Очищаем BSS секцию
    la t0, __bss_start
    la t1, __bss_end
    bgeu t0, t1, 2f
    
    // Очищаем память
    sw zero, 0(t0)
    addi t0, t0, 4
    bltu t0, t1, 1b
    
2:  // Переходим в C код
    call kernel_main
    
    // Если kernel_main вернулся, переходим в бесконечный цикл
    j 1f
    
1:  // Бесконечный цикл для всех CPU
    wfi
    j 1b

// Бесконечный цикл
halt:
    wfi
    j halt

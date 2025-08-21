// entry.S - точка входа для ARM64 архитектуры
// Код написан для ассемблера GNU Assembler (GAS)

.section .text.boot
.global _start

_start:
    // Отключаем прерывания
    msr daifset, #0xf
    
    // Получаем ID процессора
    mrs x0, mpidr_el1
    and x0, x0, #0xFF
    cbz x0, 2f
    
    // Если это не CPU0, переходим в бесконечный цикл
1:  wfe
    b 1b
    
2:  // Инициализируем стек для CPU0
    ldr x1, =_start
    mov sp, x1
    
    // Очищаем BSS секцию
    ldr x1, =__bss_start
    ldr x2, =__bss_end
    sub x2, x2, x1
    bl memzero
    
    // Переходим в C код
    bl kernel_main
    
    // Если kernel_main вернулся, переходим в бесконечный цикл
    b 1b

// Функция для очистки памяти
memzero:
    str xzr, [x0], #8
    subs x1, x1, #8
    b.hi memzero
    ret

// Бесконечный цикл
halt:
    wfe
    b halt

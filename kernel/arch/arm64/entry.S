// entry.S - точка входа для ARM64 архитектуры
// Код написан для ассемблера GNU Assembler (GAS)

.section .text.boot
.global _start

// Макрос для вывода символа в UART (для отладки)
// использование: uart_putc x0, w1  (x0 = UART address, w1 = char)
.macro uart_putc uart_addr, char_reg
    str \char_reg, [\uart_addr]
    // Небольшой delay
    mov w2, #1000
1:  subs w2, w2, #1
    b.ne 1b
.endm

_start:
    // Отключаем прерывания
    msr daifset, #0xf

    // UART адрес для QEMU virt ARM64
    ldr x0, =0x09000000

    // Выводим 'A' - вход в _start
    mov w1, #65  // 'A'
    uart_putc x0, w1

    // Получаем ID процессора
    mrs x9, mpidr_el1
    and x9, x9, #0xFF
    cbz x9, 2f

    // Выводим 'S' - slave CPU, переход в halt
    mov w1, #83  // 'S'
    uart_putc x0, w1

    // Если это не CPU0, переходим в бесконечный цикл
1:  wfe
    b 1b

2:  // Выводим 'B' - инициализация BSS
    mov w1, #66  // 'B'
    uart_putc x0, w1

    // Инициализируем стек для CPU0
    // Стек растёт вниз (от больших адресов к меньшим)
    ldr x1, =_start
    mov sp, x1
    sub sp, sp, #0x4000       // Резервируем 16KB для стека

    // Выводим 'b' - начало очистки BSS
    mov w1, #98  // 'b'
    uart_putc x0, w1

    // Очищаем BSS секцию
    ldr x1, =__bss_start
    ldr x2, =__bss_end
    sub x2, x2, x1

    // Выводим 's' - BSS size loaded
    mov w1, #115  // 's'
    uart_putc x0, w1

    cbz x2, 3f

    // Выводим 'm' - вызов memzero
    mov w1, #109  // 'm'
    uart_putc x0, w1

    bl memzero

    // Выводим 'z' - memzero завершена
    mov w1, #122  // 'z'
    uart_putc x0, w1

3:  // Выводим 'C' - переход в C код
    mov w1, #67  // 'C'
    uart_putc x0, w1

    // Переходим в kernel_main
    bl kernel_main

    // Выводим 'X' - выход из kernel_main
    mov w1, #88  // 'X'
    uart_putc x0, w1

    // Если kernel_main вернулся, переходим в бесконечный цикл
    b 1b

// Функция для очистки памяти (memset(ptr, 0, size))
// x0 = адрес, x1 = размер
memzero:
    cbz x1, memzero_done
    str xzr, [x0], #8
    subs x1, x1, #8
    b.hi memzero

memzero_done:
    ret

// Бесконечный цикл
halt:
    wfe
    b halt

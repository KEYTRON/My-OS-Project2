; entry.S - точка входа для Multiboot (остаём в 32-bit режиме)
; Код написан для ассемблера NASM

BITS 32

; --------------------------------------------------------------------
; Multiboot header
; --------------------------------------------------------------------
section .multiboot
align 4
multiboot_header:
    dd 0x1BADB002           ; magic number
    dd 0x00000000           ; flags
    dd -(0x1BADB002 + 0)    ; checksum

; --------------------------------------------------------------------
; Начало кода - остаём в 32-bit режиме
; --------------------------------------------------------------------
section .text
global _start

_start:
    cli                     ; Выключаем прерывания

    ; Установим базовый stack
    mov esp, stack_top

    ; Вызываем kernel_main (написано на C для 32-bit)
    extern kernel_main
    call kernel_main

    ; Если вернулись, то зацикливаемся
halt:
    hlt
    jmp halt

; Stack для 32-bit режима
section .bss
align 16
stack_bottom:
    resb 4096
stack_top:

; Конец entry.S

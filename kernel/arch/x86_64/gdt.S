; gdt.S — ассемблерные функции для работы с GDT и IDT

global load_gdt
global load_idt

; Функция загрузки GDT
; void load_gdt(void* gdt_ptr)
load_gdt:
    lgdt [rdi]      ; Загружаем GDT из указателя в rdi
    ret

; Функция загрузки IDT  
; void load_idt(void* idt_ptr)
load_idt:
    lidt [rdi]      ; Загружаем IDT из указателя в rdi
    ret

; isr_stubs.S — ассемблерные заглушки для обработчиков прерываний

extern isr_handler
extern irq_handler

; Макрос для создания ISR заглушки
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli                     ; Отключаем прерывания
    push qword 0            ; Пустой error code
    push qword %1           ; Номер прерывания
    jmp isr_common_stub
%endmacro

; Макрос для создания ISR заглушки с error code
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli                     ; Отключаем прерывания
    push qword %1           ; Номер прерывания (error code уже в стеке)
    jmp isr_common_stub
%endmacro

; Макрос для создания IRQ заглушки
%macro IRQ 2
global irq%1
irq%1:
    cli                     ; Отключаем прерывания
    push qword 0            ; Пустой error code
    push qword %2           ; Номер IRQ + 32
    jmp irq_common_stub
%endmacro

; Создаем ISR заглушки (0-31)
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
ISR_NOERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

; Создаем IRQ заглушки (32-47)
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

; Общий обработчик для ISR
isr_common_stub:
    ; Сохраняем все регистры
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ; Вызываем C обработчик
    mov rdi, rsp            ; Передаем указатель на структуру регистров
    call isr_handler
    
    ; Восстанавливаем регистры
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    
    ; Убираем error code и номер прерывания
    add rsp, 16
    
    sti                     ; Включаем прерывания
    iretq

; Общий обработчик для IRQ
irq_common_stub:
    ; Сохраняем все регистры
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ; Вызываем C обработчик
    mov rdi, rsp            ; Передаем указатель на структуру регистров
    call irq_handler
    
    ; Восстанавливаем регистры
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    
    ; Убираем error code и номер прерывания
    add rsp, 16
    
    sti                     ; Включаем прерывания
    iretq

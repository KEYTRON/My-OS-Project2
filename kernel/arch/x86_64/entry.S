; entry.S - точка входа, когда GRUB передает управление (Multiboot)
; Код написан для ассемблера NASM

BITS 32

; --------------------------------------------------------------------
; Начало кода
; --------------------------------------------------------------------
section .text
global _start

; Multiboot header должен быть в самом начале секции .text
align 4
multiboot_header:
    dd 0x1BADB002           ; magic number
    dd 0x00                 ; flags (не требуется гарантированный модуль)
    dd -(0x1BADB002)        ; checksum

_start:
    cli                     ; Выключаем прерывания на всякий случай

    ; Настраиваем базовую GDT для 32-битного режима
    lgdt [gdt_descriptor]   ; Загружаем GDT

    ; Переходим в 64-битный режим:
    ; 1) Включаем PAE (Physical Address Extension) через CR4
    ; 2) Включаем LME (Long Mode Enable) в MSR EFER
    ; 3) Настраиваем базовые таблицы страниц
    ; 4) Включаем пейджинг (CR0.PG = 1)
    ; 5) Переходим по far jump в 64-битный сегмент

    ; 1) Включаем PAE: CR4.PAE = 1
    mov eax, cr4
    or eax, 1 << 5           ; бит 5 = PAE (Physical Address Extension)
    mov cr4, eax

    ; 2) Включаем LME (Long Mode Enable) в MSR EFER (MSR index 0xC0000080)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8           ; EFER.LME = 1
    wrmsr

    ; 3) Настраиваем базовые таблицы страниц (1:1 отображение для первых 2MB)
    ; Обнуляем таблицы
    mov edi, pml4_table
    mov ecx, 1024            ; 512 * 8 байт = 4096 байт, делим на 4 = 1024
    xor eax, eax
    rep stosd
    
    mov edi, pdp_table
    mov ecx, 1024
    rep stosd
    
    mov edi, pd_table
    mov ecx, 1024
    rep stosd
    
    ; PML4[0] -> PDPT[0]
    mov eax, pdp_table
    or eax, 0x03             ; present=1, rw=1
    mov [pml4_table], eax
    
    ; PDPT[0] -> PD[0]
    mov eax, pd_table
    or eax, 0x03             ; present=1, rw=1
    mov [pdp_table], eax
    
    ; PD[0] -> 2MB страница (первые 2MB отображаются 1:1)
    mov eax, 0x00000083      ; present=1, rw=1, PS=1 (2MB страница)
    mov [pd_table], eax
    
    ; 4) Загружаем CR3 и включаем пейджинг
    mov eax, pml4_table
    mov cr3, eax
    
    mov eax, cr0
    or eax, 1 << 31          ; CR0.PG = 1 (включаем пейджинг)
    mov cr0, eax

    ; 5) Переходим в 64-битный режим
    jmp 0x08:long_mode_entry ; far jump: сегмент 0x08 - наш 64-битный кодовый сегмент

[BITS 64]
long_mode_entry:
    ; Устанавливаем сегменты для 64-битного режима
    mov ax, 0x10             ; сегмент данных (0x10) 
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Теперь у нас 64-битный режим, можно вызвать kernel_main
    extern kernel_main       ; объявляем, что kernel_main реализован в C
    call kernel_main

halt:
    hlt
    jmp halt

; --------------------------------------------------------------------
; Таблицы страниц (выровнены на 4KB)
; --------------------------------------------------------------------
section .data
align 4096
pml4_table:
    times 512 dq 0

align 4096
pdp_table:
    times 512 dq 0

align 4096
pd_table:
    times 512 dq 0

; --------------------------------------------------------------------
; GDT для перехода в Long Mode
; --------------------------------------------------------------------
section .text
gdt_start:
    dq 0x0000000000000000    ; NULL-сегмент
    dq 0x0020980000000000    ; Кодовый сегмент 64-бит (L=1, D=0)
    dq 0x0000920000000000    ; Данные сегмент 64-бит
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1   ; размер таблицы (limit)
    dq gdt_start                 ; базовый адрес таблицы

; Конец entry.S

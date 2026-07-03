; entry.S - 32-bit multiboot entry that switches to x86_64 long mode
;
; GRUB загружает нас в 32-битном protected mode. Здесь мы:
;   1. Сохраняем magic/указатель multiboot info (EAX/EBX)
;   2. Строим bootstrap-таблицы страниц: identity-map первых 4 GiB
;      2-МиБ страницами (ядро, VGA, LAPIC, framebuffer — всё доступно)
;   3. Включаем PAE + long mode + paging
;   4. Прыгаем в 64-битный код и вызываем kernel_main(magic, mb_info)

BITS 32

MB_FLAG_ALIGN  equ 1 << 0            ; выравнивать модули по страницам
MB_FLAG_MEM    equ 1 << 1            ; передать карту памяти
MB_FLAG_VIDEO  equ 1 << 2            ; запросить видеорежим
MB_FLAGS       equ MB_FLAG_ALIGN | MB_FLAG_MEM | MB_FLAG_VIDEO
MB_MAGIC       equ 0x1BADB002

section .multiboot
align 4
multiboot_header:
    dd MB_MAGIC
    dd MB_FLAGS
    dd -(MB_MAGIC + MB_FLAGS)
    dd 0, 0, 0, 0, 0                 ; header_addr .. entry_addr (не используются)
    dd 0                             ; mode_type: 0 = линейный графический
    dd 1024                          ; желаемая ширина
    dd 768                           ; желаемая высота
    dd 32                            ; желаемая глубина цвета

section .text
global _start
extern kernel_main

_start:
    cli

    ; temporary 32-bit stack
    mov esp, stack32_top

    ; сохраняем регистры multiboot до того, как их затрём
    mov [mb_magic_saved], eax
    mov [mb_info_saved], ebx

    ; clear bootstrap page tables (PML4 + PDPT + 4 PD)
    lea edi, [pml4_table]
    mov ecx, (4096 * 6) / 4
    xor eax, eax
    rep stosd

    ; PML4[0] -> PDPT
    mov eax, pdpt_table
    or eax, 0x3                      ; present | writable
    mov [pml4_table], eax

    ; PDPT[0..3] -> PD0..PD3 (по 1 GiB каждая)
    mov eax, pd_tables
    or eax, 0x3
    mov edi, pdpt_table
    mov ecx, 4
.fill_pdpt:
    mov [edi], eax
    add eax, 4096
    add edi, 8
    loop .fill_pdpt

    ; заполняем 4 PD: 4 * 512 записей по 2 MiB = 4 GiB identity map
    mov edi, pd_tables
    mov eax, 0x00000083              ; present | writable | 2 MiB page
    mov ecx, 512 * 4
.fill_pd:
    mov [edi], eax
    add eax, 0x200000                ; следующие 2 MiB
    add edi, 8
    loop .fill_pd

    ; enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; load PML4 base
    mov eax, pml4_table
    mov cr3, eax

    ; enable long mode in EFER
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8                   ; LME
    wrmsr

    ; enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; load temporary long-mode GDT
    lgdt [gdt64_ptr]

    ; jump to 64-bit mode
    jmp 0x08:long_mode_entry

[BITS 64]
long_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov rsp, stack64_top
    xor rbp, rbp

    ; kernel_main(uint32_t magic, uint32_t mb_info)
    mov edi, [mb_magic_saved]
    mov esi, [mb_info_saved]

    call kernel_main

.hang:
    hlt
    jmp .hang

section .data
align 8
gdt64:
    dq 0x0000000000000000              ; null
    dq 0x00AF9A000000FFFF              ; 64-bit kernel code (L=1, G=1)
    dq 0x008F92000000FFFF              ; kernel data (L=0, G=1)
gdt64_end:

gdt64_ptr:
    dw gdt64_end - gdt64 - 1
    dd gdt64

mb_magic_saved: dd 0
mb_info_saved:  dd 0

section .bss
align 4096
pml4_table:
    resb 4096
pdpt_table:
    resb 4096
pd_tables:
    resb 4096 * 4

align 16
stack32:
    resb 4096
stack32_top:

align 16
stack64:
    resb 16384
stack64_top:

section .note.GNU-stack noalloc noexec nowrite progbits

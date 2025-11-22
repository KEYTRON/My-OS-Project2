; entry.S - 32-bit multiboot entry that switches to x86_64 long mode

BITS 32

section .multiboot
align 4
multiboot_header:
    dd 0x1BADB002
    dd 0x00000000
    dd -(0x1BADB002 + 0)

section .text
global _start
extern kernel_main

_start:
    cli

    ; temporary 32-bit stack
    mov esp, stack32_top

    ; clear bootstrap page tables
    lea edi, [pml4_table]
    mov ecx, (4096 * 3) / 4
    xor eax, eax
    rep stosd

    ; identity-map first 2 MiB using a single 2 MiB page
    mov eax, pdpt_table
    or eax, 0x3
    mov [pml4_table], eax
    mov dword [pml4_table + 4], 0

    mov eax, pd_table
    or eax, 0x3
    mov [pdpt_table], eax
    mov dword [pdpt_table + 4], 0

    mov eax, 0x00000083        ; present | writable | 2 MiB page
    mov [pd_table], eax
    mov dword [pd_table + 4], 0

    ; enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; load PML4 base
    mov eax, pml4_table
    mov cr3, eax

    ; enable long mode in EFER (NXE commented out for now)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8             ; LME
    ;or eax, 1 << 11           ; NXE
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

section .bss
align 4096
pml4_table:
    resb 4096
pdpt_table:
    resb 4096
pd_table:
    resb 4096

align 16
stack32:
    resb 4096
stack32_top:

align 16
stack64:
    resb 8192
stack64_top:

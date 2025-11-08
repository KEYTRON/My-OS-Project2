; Simple Multiboot compatible loader for x86_64
; This file creates a minimal 32-bit executable that QEMU can boot

BITS 32

; Multiboot constants
MULTIBOOT_MAGIC equ 0x1BADB002
MULTIBOOT_FLAGS equ 0x00000000
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

section .multiboot
align 4
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

section .text
global _start
_start:
    cli

    ; Just halt for now - QEMU will handle the rest
    jmp $

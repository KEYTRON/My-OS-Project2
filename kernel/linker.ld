/* linker.ld — скрипт для линковки ядра */
OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS {
    /* Разместим код и данные, начиная с физического адреса 1 MiB (0x100000) */
    . = 0x100000;

    .text : {
        *(.multiboot_header)   /* Секция Multiboot‑заголовка (из entry.S) */
        *(.text)               /* Весь код */
        *(.rodata)             /* Константы, расположенные в readonly */
    }

    .data : {
        *(.data)               /* Инициализированные данные */
    }

    .bss : {
        *(.bss)
        *(COMMON)
    }
}
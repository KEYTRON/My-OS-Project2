/* linker.ld — скрипт для линковки ядра */
OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS {
    /* Размещаем код, начиная с физического адреса 1 MiB (0x100000) */
    . = 0x100000;

    .text : {
        *(.text)               /* Весь код, включая Multiboot header */
        *(.rodata)             /* Константы, расположенные в readonly */
    } :text

    .data : {
        *(.data)               /* Инициализированные данные */
    } :data

    .bss : {
        *(.bss)
        *(COMMON)
    } :data

    /DISCARD/ : {
        *(.comment)
        *(.gnu*)
        *(.note*)
        *(.eh_frame*)
    }
}

PHDRS {
    text PT_LOAD FLAGS(5);    /* PF_R | PF_X */
    data PT_LOAD FLAGS(6);    /* PF_R | PF_W */
}
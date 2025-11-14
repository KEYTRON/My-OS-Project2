/* linker.ld — скрипт для линковки ядра */
OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS {
    /* Размещаем код, начиная с физического адреса 1 MiB (0x100000) */
    . = 0x100000;

    /*
     * Multiboot header должен располагаться в пределах первых 8 KiB
     * двоичного файла. Явно выделяем секцию, чтобы компоновщик не
     * отбросил её и не переместил дальше по файлу.
     */
    .multiboot ALIGN(8) : {
        KEEP(*(.multiboot))
    } :text

    .text : {
        *(.text)               /* Весь код */
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
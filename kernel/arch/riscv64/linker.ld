/* linker.ld для RISC-V64 архитектуры */
OUTPUT_FORMAT(elf64-littleriscv)
ENTRY(_start)

SECTIONS {
    /* RISC-V64 ядро загружается по адресу 0x80000000 */
    . = 0x80000000;

    .text.boot : {
        *(.text.boot)
    }

    .text : {
        *(.text)
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        __bss_start = .;
        *(.bss)
        *(COMMON)
        __bss_end = .;
    }

    /DISCARD/ : {
        *(.comment)
        *(.gnu*)
        *(.note*)
        *(.eh_frame*)
    }
}

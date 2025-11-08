/* linker.ld для ARM64 архитектуры */
OUTPUT_FORMAT(elf64-littleaarch64)
ENTRY(_start)

SECTIONS {
    /* ARM64 ядро загружается по адресу 0x80000 (QEMU virt) */
    . = 0x80000;

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

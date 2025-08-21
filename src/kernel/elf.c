#include "elf.h"
#include "memory.h"
#include "vga.h"

struct Elf32_Ehdr {
    unsigned char e_ident[16];
    unsigned short e_type;
    unsigned short e_machine;
    unsigned int e_version;
    unsigned int e_entry;
    unsigned int e_phoff;
    unsigned int e_shoff;
    unsigned int e_flags;
    unsigned short e_ehsize;
    unsigned short e_phentsize;
    unsigned short e_phnum;
    unsigned short e_shentsize;
    unsigned short e_shnum;
    unsigned short e_shstrndx;
};

struct Elf32_Phdr {
    unsigned int p_type;
    unsigned int p_offset;
    unsigned int p_vaddr;
    unsigned int p_paddr;
    unsigned int p_filesz;
    unsigned int p_memsz;
    unsigned int p_flags;
    unsigned int p_align;
};

static int ident_ok(const unsigned char *id) {
    return id[0] == 0x7F && id[1] == 'E' && id[2] == 'L' && id[3] == 'F';
}

int elf_load(const char *data, unsigned int size, void (**entry)()) {
    if (size < sizeof(struct Elf32_Ehdr)) {
        print_string("ELF too small\n");
        return -1;
    }

    const struct Elf32_Ehdr *hdr = (const struct Elf32_Ehdr*)data;
    if (!ident_ok(hdr->e_ident)) {
        print_string("Invalid ELF header\n");
        return -1;
    }

    *entry = (void (*)())hdr->e_entry;
    const struct Elf32_Phdr *ph = (const struct Elf32_Phdr*)(data + hdr->e_phoff);

    for (int i = 0; i < hdr->e_phnum; i++) {
        if (ph[i].p_type != 1) continue; /* PT_LOAD */

        char *seg = (char*)malloc_simple(ph[i].p_memsz);
        for (unsigned int j = 0; j < ph[i].p_filesz; j++) {
            seg[j] = data[ph[i].p_offset + j];
        }
        for (unsigned int j = ph[i].p_filesz; j < ph[i].p_memsz; j++) {
            seg[j] = 0;
        }
    }

    print_string("ELF loaded\n");
    return 0;
}


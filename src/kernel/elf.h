#ifndef ELF_H
#define ELF_H

int elf_load(const char *data, unsigned int size, void (**entry)());

#endif

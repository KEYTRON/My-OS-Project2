// gdt.c - GDT setup for x86_64
#include "gdt.h"
#include "../../lib/string.h"

static struct GDTEntry gdt_entries[GDT_ENTRIES];
static struct GDTPtr   gdt_ptr;

static void set_gdt_entry(int idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[idx].limit_low    = (limit & 0xFFFF);
    gdt_entries[idx].base_low     = (base & 0xFFFF);
    gdt_entries[idx].base_middle  = (base >> 16) & 0xFF;
    gdt_entries[idx].access       = access;
    gdt_entries[idx].granularity  = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt_entries[idx].base_high    = (base >> 24) & 0xFF;
}

extern void load_gdt(void*);

void gdt_init() {
    memset(&gdt_entries, 0, sizeof(gdt_entries));

    set_gdt_entry(GDT_NULL_SEGMENT, 0, 0, 0, 0);

    set_gdt_entry(GDT_KERNEL_CODE_SEGMENT, 0x0, 0x000FFFFF, 0x9A, 0xA0); // L=1 for code
    set_gdt_entry(GDT_KERNEL_DATA_SEGMENT, 0x0, 0x000FFFFF, 0x92, 0x80); // L=0 for data

    set_gdt_entry(GDT_USER_CODE_SEGMENT, 0x0, 0x000FFFFF, 0xFA, 0xA0);
    set_gdt_entry(GDT_USER_DATA_SEGMENT, 0x0, 0x000FFFFF, 0xF2, 0x80);

    set_gdt_entry(GDT_TSS_SEGMENT, 0x0, 0x00000000, 0x89, 0x00);

    gdt_ptr.limit = sizeof(gdt_entries) - 1;
    gdt_ptr.base  = (uint64_t)&gdt_entries;

    load_gdt(&gdt_ptr);
}

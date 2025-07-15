#include "ata.h"

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}

static inline unsigned short inw(unsigned short port) {
    unsigned short ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void io_wait() {
    outb(0x80, 0);
}

void ata_init() {
    // Пока инициализация не требуется
}

void ata_read_sector(unsigned int lba, unsigned char* buffer) {
    // Ожидаем готовности диска
    while (inb(0x1F7) & 0x80);

    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1); // количество секторов
    outb(0x1F3, (unsigned char)(lba & 0xFF));
    outb(0x1F4, (unsigned char)((lba >> 8) & 0xFF));
    outb(0x1F5, (unsigned char)((lba >> 16) & 0xFF));
    outb(0x1F7, 0x20); // команда чтения

    io_wait();
    while (inb(0x1F7) & 0x80); // ожидание BSY=0
    while (!(inb(0x1F7) & 8)); // ожидание DRQ=1

    for (int i = 0; i < 256; i++) {
        ((unsigned short*)buffer)[i] = inw(0x1F0);
    }
}

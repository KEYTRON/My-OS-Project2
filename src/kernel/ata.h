#ifndef ATA_H
#define ATA_H

// Инициализация (пока пустая)
void ata_init();

// Чтение одного сектора по LBA в буфер (512 байт)
void ata_read_sector(unsigned int lba, unsigned char* buffer);

#endif

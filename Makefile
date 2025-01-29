# Makefile

# Путь к компилятору
CC = i386-elf-gcc
LD = i386-elf-ld
ASM = nasm
CFLAGS = -m32 -ffreestanding -O2 -nostdlib
LDFLAGS = -T kernel/kernel.ld

all: os-image

# Компиляция загрузчика
boot/boot.bin: boot/boot.asm
	$(ASM) -f bin $< -o $@

# Компиляция ядра
kernel/kernel.bin: kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

# Компоновка ядра
kernel/kernel.elf: kernel/kernel.bin
	$(LD) $(LDFLAGS) -o $@ $<

# Создание образа ОС
os-image: boot/boot.bin kernel/kernel.elf
	cat $^ > os-image.bin

clean:
	rm -f boot/*.bin kernel/*.bin kernel/*.elf os-image.bin
# Makefile

BOOT_DIR := boot
KERNEL_DIR := kernel
BUILD_DIR := build

$(shell mkdir -p $(BUILD_DIR))

# Путь к компилятору
CC = i386-elf-gcc
LD = i386-elf-ld
ASM = nasm
CFLAGS = -m32 -ffreestanding -O2 -nostdlib
LDFLAGS = -T kernel/kernel.ld

all: os-image

# Компиляция загрузчика
$(BOOT_DIR)/boot.bin: $(BOOT_DIR)/boot.asm
	$(ASM) -f bin $< -o $@

$(BUILD_DIR)/kernel.o: $(KERNEL_DIR)/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel.elf: $(BUILD_DIR)/kernel.o
	$(LD) $(LDFLAGS) -o $@ $<

# Создание образа ОС
os-image: $(BOOT_DIR)/boot.bin $(BUILD_DIR)/kernel.elf
	cat $^ > os-image.bin

run: os-image
	qemu-system-i386 -drive format=raw,file=os-image.bin

clean:
	rm -f $(BOOT_DIR)/*.bin $(BUILD_DIR)/* os-image.bin
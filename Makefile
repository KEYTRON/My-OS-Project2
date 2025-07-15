# Makefile

BOOT_DIR := src/bootloader
KERNEL_DIR := src/kernel
BUILD_DIR := build

$(shell mkdir -p $(BUILD_DIR))

# Путь к компилятору
CC = i686-linux-gnu-gcc
LD = i686-linux-gnu-ld
ASM = nasm
CFLAGS = -m32 -ffreestanding -O2 -nostdlib
LDFLAGS = -T $(KERNEL_DIR)/kernel.ld

all: os-image

# Компиляция загрузчика
$(BOOT_DIR)/boot.bin: $(BOOT_DIR)/boot.asm
	$(ASM) -f bin $< -o $@

$(BUILD_DIR)/kernel.o: $(KERNEL_DIR)/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/vga.o: $(KERNEL_DIR)/vga.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/memory.o: $(KERNEL_DIR)/memory.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/interrupts.o: $(KERNEL_DIR)/interrupts.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/keyboard.o: $(KERNEL_DIR)/keyboard.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/paging.o: $(KERNEL_DIR)/paging.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/isr.o: $(KERNEL_DIR)/isr.S
	$(CC) -c $< -o $@

OBJS := $(BUILD_DIR)/kernel.o $(BUILD_DIR)/vga.o \
$(BUILD_DIR)/memory.o $(BUILD_DIR)/interrupts.o \
$(BUILD_DIR)/keyboard.o $(BUILD_DIR)/paging.o $(BUILD_DIR)/isr.o

$(BUILD_DIR)/kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# Создание образа ОС
os-image: $(BOOT_DIR)/boot.bin $(BUILD_DIR)/kernel.elf
	cat $^ > os-image.bin

run: os-image
	qemu-system-i386 -drive format=raw,file=os-image.bin

clean:
	rm -f $(BOOT_DIR)/*.bin $(BUILD_DIR)/* os-image.bin

# Makefile

# Пути
BOOT_DIR := src/bootloader
KERNEL_DIR := src/kernel
BUILD_DIR := build

# Создание каталога сборки
$(shell mkdir -p $(BUILD_DIR))

# Компиляторы и флаги
CC := i686-linux-gnu-gcc
LD := i686-linux-gnu-ld
ASM := nasm
CFLAGS := -m32 -ffreestanding -O2 -nostdlib
LDFLAGS := -T $(KERNEL_DIR)/kernel.ld

# Файлы
BOOT_BIN := $(BOOT_DIR)/boot.bin
KERNEL_ELF := $(BUILD_DIR)/kernel.elf
OS_IMAGE := $(BUILD_DIR)/os-image.bin

# Object-файлы
OBJS := \
    $(BUILD_DIR)/kernel.o \
    $(BUILD_DIR)/vga.o \
    $(BUILD_DIR)/memory.o \
    $(BUILD_DIR)/interrupts.o \
    $(BUILD_DIR)/keyboard.o \
    $(BUILD_DIR)/paging.o \
    $(BUILD_DIR)/fs.o \
    $(BUILD_DIR)/shell.o \
    $(BUILD_DIR)/task.o \
    $(BUILD_DIR)/ata.o \
    $(BUILD_DIR)/elf.o \
    $(BUILD_DIR)/isr.o

# Основная цель
all: $(OS_IMAGE)

# Сборка загрузчика
$(BOOT_BIN): $(BOOT_DIR)/boot.asm
	$(ASM) -f bin $< -o $@

# Компиляция C-файлов
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c
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

$(BUILD_DIR)/fs.o: $(KERNEL_DIR)/fs.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/elf.o: $(KERNEL_DIR)/elf.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/ata.o: $(KERNEL_DIR)/ata.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/shell.o: $(KERNEL_DIR)/shell.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/task.o: $(KERNEL_DIR)/task.c
	$(CC) $(CFLAGS) -c $< -o $@

OBJS := $(BUILD_DIR)/kernel.o $(BUILD_DIR)/vga.o \
    $(BUILD_DIR)/memory.o $(BUILD_DIR)/interrupts.o \
    $(BUILD_DIR)/keyboard.o $(BUILD_DIR)/paging.o $(BUILD_DIR)/fs.o \
    $(BUILD_DIR)/shell.o $(BUILD_DIR)/task.o $(BUILD_DIR)/ata.o \
    $(BUILD_DIR)/elf.o $(BUILD_DIR)/isr.o

# Компиляция ассемблера
$(BUILD_DIR)/isr.o: $(KERNEL_DIR)/isr.S
	$(CC) $(CFLAGS) -c $< -o $@

# Линковка ядра
$(KERNEL_ELF): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# Создание финального образа ОС
$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_ELF)
	cat $^ > $@

# Очистка
clean:
	rm -f $(BOOT_DIR)/*.bin $(BUILD_DIR)/* $(OS_IMAGE)

.PHONY: all clean

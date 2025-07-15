# Пути
BOOT_DIR := src/bootloader
KERNEL_DIR := src/kernel
BUILD_DIR := build

# Компиляторы и флаги
CC := i686-linux-gnu-gcc
LD := i686-linux-gnu-ld
ASM := nasm
CFLAGS := -m32 -ffreestanding -O2 -nostdlib
LDFLAGS := -T $(KERNEL_DIR)/kernel.ld

# Файлы ядра
C_SOURCES := $(wildcard $(KERNEL_DIR)/*.c)
OBJ_FILES := $(patsubst $(KERNEL_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))

# Целевой файл
KERNEL_ELF := $(BUILD_DIR)/kernel.elf
BOOT_BIN := $(BOOT_DIR)/boot.bin
OS_IMAGE := $(BUILD_DIR)/os-image.bin

# Создание каталога сборки
$(shell mkdir -p $(BUILD_DIR))

# Основная цель
all: $(OS_IMAGE)

# Сборка загрузчика
$(BOOT_BIN): $(BOOT_DIR)/boot.asm
	$(ASM) -f bin $< -o $@

# Компиляция .c файлов в .o
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Линковка ядра
$(KERNEL_ELF): $(OBJ_FILES)
	$(LD) $(LDFLAGS) -o $@ $^

# Создание финального образа ОС
$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_ELF)
	cat $^ > $@

# Очистка
clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all clean
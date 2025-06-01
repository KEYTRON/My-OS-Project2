# Makefile для ядра

# Кросс‑компилятор
CC      := x86_64-elf-gcc
LD      := x86_64-elf-ld
NASM    := nasm

# Флаги для компилятора
CFLAGS  := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iinclude
LDFLAGS := -nostdlib -T linker.ld

# Папка с исходниками ядра
SRCDIR  := .
OUTDIR  := build

# Список C‑файлов (рекурсивно)
C_SRCS := $(shell find arch/x86_64 -name '*.c') \
          $(shell find drivers -name '*.c') \
          $(shell find lib -name '*.c') \
          kmain.c

# Соответствующие объектные файлы
C_OBJS := $(patsubst %.c, $(OUTDIR)/%.o, $(C_SRCS))

# Ассемблерный файл
ASM_SRCS := arch/x86_64/entry.S

# Объект файла из ассемблера
ASM_OBJS := $(patsubst %.S, $(OUTDIR)/%.o, $(ASM_SRCS))

# Файл результата
KERNEL_BIN := $(OUTDIR)/kernel.bin

# Правила
all: $(KERNEL_BIN)

# Собираем C‑файлы
$(OUTDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Собираем ассемблер (NASM)
$(OUTDIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(NASM) -f elf64 $< -o $@

# Линкуем ядро
$(KERNEL_BIN): $(ASM_OBJS) $(C_OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(ASM_OBJS) $(C_OBJS)

# Очистка
clean:
	rm -rf $(OUTDIR)

.PHONY: all clean
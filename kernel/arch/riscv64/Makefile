# Makefile для RISC-V64 архитектуры

CC      := riscv64-elf-gcc
LD      := riscv64-elf-ld
AS      := riscv64-elf-as

# Флаги для компилятора
CFLAGS  := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I../../include -I../riscv64
LDFLAGS := -nostdlib -T linker.ld

# Папка с исходниками
SRCDIR  := .
OUTDIR  := build

# Список C-файлов
C_SRCS := $(shell find . -name '*.c') \
          $(shell find ../../drivers -name '*.c') \
          $(shell find ../../lib -name '*.c') \
          ../../kmain.c

# Соответствующие объектные файлы
C_OBJS := $(patsubst %.c, $(OUTDIR)/%.o, $(C_SRCS))

# Ассемблерные файлы
ASM_SRCS := entry.S

# Объект файлы из ассемблера
ASM_OBJS := $(patsubst %.S, $(OUTDIR)/%.o, $(ASM_SRCS))

# Файл результата
KERNEL_BIN := $(OUTDIR)/kernel-riscv64.bin

# Правила
all: $(KERNEL_BIN)

# Собираем C-файлы
$(OUTDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Собираем ассемблер
$(OUTDIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(AS) -o $@ $<

# Линкуем ядро
$(KERNEL_BIN): $(ASM_OBJS) $(C_OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(ASM_OBJS) $(C_OBJS)

# Очистка
clean:
	rm -rf $(OUTDIR)

.PHONY: all clean

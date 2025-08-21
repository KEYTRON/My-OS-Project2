# Makefile для ядра MyOS - поддержка множественных архитектур

# Определяем архитектуру по умолчанию
ARCH ?= x86_64

# Кросс-компиляторы для разных архитектур
X86_64_CC := x86_64-elf-gcc
X86_64_LD := x86_64-elf-ld
X86_64_NASM := nasm

ARM64_CC := aarch64-elf-gcc
ARM64_LD := aarch64-elf-ld
ARM64_AS := aarch64-elf-as

RISCV64_CC := riscv64-elf-gcc
RISCV64_LD := riscv64-elf-ld
RISCV64_AS := riscv64-elf-as

# Выбираем компиляторы в зависимости от архитектуры
ifeq ($(ARCH),x86_64)
    CC := $(X86_64_CC)
    LD := $(X86_64_LD)
    NASM := $(X86_64_NASM)
    KERNEL_BIN := build/kernel-x86_64.bin
    LINKER_SCRIPT := linker.ld
else ifeq ($(ARCH),arm64)
    CC := $(ARM64_CC)
    LD := $(ARM64_LD)
    AS := $(ARM64_AS)
    KERNEL_BIN := build/kernel-arm64.bin
    LINKER_SCRIPT := arch/arm64/linker.ld
else ifeq ($(ARCH),riscv64)
    CC := $(RISCV64_CC)
    LD := $(RISCV64_LD)
    AS := $(RISCV64_AS)
    KERNEL_BIN := build/kernel-riscv64.bin
    LINKER_SCRIPT := arch/riscv64/linker.ld
else
    $(error Unsupported architecture: $(ARCH). Supported: x86_64, arm64, riscv64)
endif

# Флаги для компилятора
CFLAGS  := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iinclude
LDFLAGS := -nostdlib -T $(LINKER_SCRIPT)

# Папка с исходниками ядра
SRCDIR  := .
OUTDIR  := build

# Список C-файлов (архитектурно-независимых)
C_SRCS := $(shell find drivers -name '*.c') \
          $(shell find lib -name '*.c') \
          kmain.c

# Архитектурно-зависимые C-файлы
ifeq ($(ARCH),x86_64)
    ARCH_C_SRCS := arch/x86_64/gdt.c \
                   arch/x86_64/idt.c \
                   arch/x86_64/isr.c \
                   arch/x86_64/paging.c
else ifeq ($(ARCH),arm64)
    ARCH_C_SRCS := # ARM64 специфичные C-файлы будут добавлены позже
else ifeq ($(ARCH),riscv64)
    ARCH_C_SRCS := # RISC-V64 специфичные C-файлы будут добавлены позже
endif

# Объединяем все C-файлы
ALL_C_SRCS := $(C_SRCS) $(ARCH_C_SRCS)

# Соответствующие объектные файлы
C_OBJS := $(patsubst %.c, $(OUTDIR)/%.o, $(ALL_C_SRCS))

# Архитектурно-зависимые файлы
ifeq ($(ARCH),x86_64)
    # x86_64 специфичные файлы
    ASM_SRCS := arch/x86_64/entry.S \
                arch/x86_64/isr_stubs.S
    
    ASM_OBJS := $(patsubst %.S, $(OUTDIR)/%.o, $(ASM_SRCS))
    GDT_ASM_OBJ := $(OUTDIR)/arch/x86_64/gdt_asm.o
    
    ARCH_OBJS := $(ASM_OBJS) $(GDT_ASM_OBJ)
    
else ifeq ($(ARCH),arm64)
    # ARM64 специфичные файлы
    ASM_SRCS := arch/arm64/entry.S
    
    ASM_OBJS := $(patsubst %.S, $(OUTDIR)/%.o, $(ASM_SRCS))
    
    ARCH_OBJS := $(ASM_OBJS)
    
else ifeq ($(ARCH),riscv64)
    # RISC-V64 специфичные файлы
    ASM_SRCS := arch/riscv64/entry.S
    
    ASM_OBJS := $(patsubst %.S, $(OUTDIR)/%.o, $(ASM_SRCS))
    
    ARCH_OBJS := $(ASM_OBJS)
endif

# Правила
all: $(KERNEL_BIN)

# Собираем C-файлы
$(OUTDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Архитектурно-зависимые правила для ассемблерных файлов
ifeq ($(ARCH),x86_64)
    # Правила для x86_64 (NASM)
    $(OUTDIR)/%.o: %.S
		@mkdir -p $(dir $@)
		$(NASM) -f elf64 $< -o $@
    
    $(GDT_ASM_OBJ): arch/x86_64/gdt.S
		@mkdir -p $(dir $@)
		$(NASM) -f elf64 $< -o $@
else
    # Правила для ARM64 и RISC-V64 (GAS)
    $(OUTDIR)/%.o: %.S
		@mkdir -p $(dir $@)
		$(AS) -o $@ $<
endif

# Линкуем ядро
$(KERNEL_BIN): $(ARCH_OBJS) $(C_OBJS) $(LINKER_SCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(ARCH_OBJS) $(C_OBJS)

# Сборка для всех архитектур
all-archs:
	$(MAKE) ARCH=x86_64
	$(MAKE) ARCH=arm64
	$(MAKE) ARCH=riscv64

# Очистка
clean:
	rm -rf $(OUTDIR)

# Очистка для всех архитектур
clean-all:
	rm -rf build
	rm -rf arch/*/build

# Проверка зависимостей
check-deps:
	@echo "Checking dependencies for $(ARCH)..."
ifeq ($(ARCH),x86_64)
	@which $(X86_64_CC) > /dev/null || (echo "Error: $(X86_64_CC) not found" && exit 1)
	@which $(X86_64_LD) > /dev/null || (echo "Error: $(X86_64_LD) not found" && exit 1)
	@which $(X86_64_NASM) > /dev/null || (echo "Error: $(X86_64_NASM) not found" && exit 1)
else ifeq ($(ARCH),arm64)
	@which $(ARM64_CC) > /dev/null || (echo "Error: $(ARM64_CC) not found" && exit 1)
	@which $(ARM64_LD) > /dev/null || (echo "Error: $(ARM64_LD) not found" && exit 1)
	@which $(ARM64_AS) > /dev/null || (echo "Error: $(ARM64_AS) not found" && exit 1)
else ifeq ($(ARCH),riscv64)
	@which $(RISCV64_CC) > /dev/null || (echo "Error: $(RISCV64_CC) not found" && exit 1)
	@which $(RISCV64_LD) > /dev/null || (echo "Error: $(RISCV64_LD) not found" && exit 1)
	@which $(RISCV64_AS) > /dev/null || (echo "Error: $(RISCV64_AS) not found" && exit 1)
endif
	@echo "Dependencies OK for $(ARCH)"

.PHONY: all all-archs clean clean-all check-deps
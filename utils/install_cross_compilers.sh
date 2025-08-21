#!/bin/bash
# install_cross_compilers.sh - установка кросс-компиляторов для всех архитектур

echo "=== Установка кросс-компиляторов для MyOS ==="
echo ""

# Проверяем операционную систему
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Обнаружена macOS, используем Homebrew..."
    PACKAGE_MANAGER="brew"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "Обнаружен Linux, используем apt..."
    PACKAGE_MANAGER="apt"
else
    echo "Неподдерживаемая операционная система: $OSTYPE"
    exit 1
fi

# Функция для установки пакетов
install_packages() {
    local packages=("$@")
    
    if [[ "$PACKAGE_MANAGER" == "brew" ]]; then
        for package in "${packages[@]}"; do
            echo "Устанавливаем $package..."
            brew install "$package" || echo "Ошибка установки $package"
        done
    elif [[ "$PACKAGE_MANAGER" == "apt" ]]; then
        sudo apt update
        for package in "${packages[@]}"; do
            echo "Устанавливаем $package..."
            sudo apt install -y "$package" || echo "Ошибка установки $package"
        done
    fi
}

# Устанавливаем x86_64 кросс-компилятор
echo "1. Установка x86_64 кросс-компилятора..."
if [[ "$PACKAGE_MANAGER" == "brew" ]]; then
    install_packages "x86_64-elf-gcc" "x86_64-elf-binutils" "nasm"
elif [[ "$PACKAGE_MANAGER" == "apt" ]]; then
    install_packages "gcc-multilib" "g++-multilib" "nasm"
fi

# Устанавливаем ARM64 кросс-компилятор
echo ""
echo "2. Установка ARM64 кросс-компилятора..."
if [[ "$PACKAGE_MANAGER" == "brew" ]]; then
    install_packages "arm-none-eabi-gcc" "arm-none-eabi-binutils"
elif [[ "$PACKAGE_MANAGER" == "apt" ]]; then
    install_packages "gcc-aarch64-linux-gnu" "g++-aarch64-linux-gnu" "binutils-aarch64-linux-gnu"
fi

# Устанавливаем RISC-V64 кросс-компилятор
echo ""
echo "3. Установка RISC-V64 кросс-компилятора..."
if [[ "$PACKAGE_MANAGER" == "brew" ]]; then
    install_packages "riscv-gnu-toolchain"
elif [[ "$PACKAGE_MANAGER" == "apt" ]]; then
    install_packages "gcc-riscv64-linux-gnu" "g++-riscv64-linux-gnu" "binutils-riscv64-linux-gnu"
fi

# Устанавливаем дополнительные инструменты
echo ""
echo "4. Установка дополнительных инструментов..."
if [[ "$PACKAGE_MANAGER" == "brew" ]]; then
    install_packages "qemu" "xorriso"
elif [[ "$PACKAGE_MANAGER" == "apt" ]]; then
    install_packages "qemu-system-x86" "qemu-system-arm" "qemu-system-riscv64" "xorriso"
fi

echo ""
echo "=== Проверка установленных компиляторов ==="

# Проверяем x86_64
echo "Проверяем x86_64 компилятор..."
if command -v x86_64-elf-gcc &> /dev/null; then
    echo "✓ x86_64-elf-gcc: $(x86_64-elf-gcc --version | head -1)"
else
    echo "✗ x86_64-elf-gcc не найден"
fi

# Проверяем ARM64
echo "Проверяем ARM64 компилятор..."
if command -v aarch64-elf-gcc &> /dev/null; then
    echo "✓ aarch64-elf-gcc: $(aarch64-elf-gcc --version | head -1)"
elif command -v aarch64-linux-gnu-gcc &> /dev/null; then
    echo "✓ aarch64-linux-gnu-gcc: $(aarch64-linux-gnu-gcc --version | head -1)"
else
    echo "✗ ARM64 компилятор не найден"
fi

# Проверяем RISC-V64
echo "Проверяем RISC-V64 компилятор..."
if command -v riscv64-elf-gcc &> /dev/null; then
    echo "✓ riscv64-elf-gcc: $(riscv64-elf-gcc --version | head -1)"
elif command -v riscv64-linux-gnu-gcc &> /dev/null; then
    echo "✓ riscv64-linux-gnu-gcc: $(riscv64-linux-gnu-gcc --version | head -1)"
else
    echo "✗ RISC-V64 компилятор не найден"
fi

echo ""
echo "=== Установка завершена ==="
echo ""
echo "Теперь вы можете собирать ядро для разных архитектур:"
echo "  make ARCH=x86_64    # для x86_64"
echo "  make ARCH=arm64     # для ARM64"
echo "  make ARCH=riscv64   # для RISC-V64"
echo "  make all-archs      # для всех архитектур"
echo ""
echo "Для проверки зависимостей используйте:"
echo "  make check-deps ARCH=<architecture>"

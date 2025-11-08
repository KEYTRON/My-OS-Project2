#!/bin/bash
# run_myos.sh - Универсальный скрипт для запуска MyOS на разных архитектурах

set -e

# Цвета
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
BOLD='\033[1m'
NC='\033[0m' # No Color

# Функции
print_header() {
    echo ""
    echo -e "${BOLD}${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BOLD}${BLUE}$1${NC}"
    echo -e "${BOLD}${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
}

print_option() {
    echo -e "${GREEN}[$1]${NC} $2"
}

print_error() {
    echo -e "${RED}❌ Error: $1${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_info() {
    echo -e "${YELLOW}ℹ $1${NC}"
}

# Проверяем наличие QEMU
check_qemu() {
    if ! command -v qemu-system-"$1" &> /dev/null; then
        print_error "qemu-system-$1 not found"
        return 1
    fi
    return 0
}

# Запуск x86_64
run_x86_64() {
    print_header "Starting MyOS on x86_64"

    ISO="./myos.iso"
    if [ ! -f "$ISO" ]; then
        print_error "$ISO not found. Please run ./build_iso.sh first"
        exit 1
    fi

    print_info "CPU: 2 cores"
    print_info "RAM: 512 MB"
    print_info "Boot: ISO ($ISO)"
    echo ""
    echo -e "${YELLOW}Controls:${NC}"
    echo "  • Press Ctrl+A then X to exit (console mode)"
    echo "  • Press Ctrl+A then C to switch to QEMU monitor"
    echo ""

    check_qemu x86_64 || { print_error "QEMU x86_64 not available"; exit 1; }

    qemu-system-x86_64 \
        -cdrom "$ISO" \
        -m 512 \
        -smp 2 \
        -serial stdio \
        -no-reboot \
        -display none \
        -enable-kvm 2>/dev/null || {

        print_info "KVM not available, running without acceleration..."
        qemu-system-x86_64 \
            -cdrom "$ISO" \
            -m 512 \
            -smp 2 \
            -serial stdio \
            -no-reboot \
            -display none
    }
}

# Запуск ARM64
run_arm64() {
    print_header "Starting MyOS on ARM64 (Cortex-A72)"

    KERNEL="./kernel/build/kernel-arm64.bin"
    if [ ! -f "$KERNEL" ]; then
        print_error "$KERNEL not found. Please build kernel first:"
        echo "  cd kernel && make ARCH=arm64"
        exit 1
    fi

    print_info "Machine: QEMU virt (ARMv8-A)"
    print_info "CPU: Cortex-A72"
    print_info "RAM: 512 MB"
    echo ""
    echo -e "${YELLOW}Controls:${NC}"
    echo "  • Press Ctrl+A then X to exit (console mode)"
    echo "  • Press Ctrl+A then C to switch to QEMU monitor"
    echo ""

    check_qemu aarch64 || { print_error "QEMU aarch64 not available"; exit 1; }

    qemu-system-aarch64 \
        -M virt \
        -cpu cortex-a72 \
        -m 512 \
        -kernel "$KERNEL" \
        -serial mon:stdio \
        -display none \
        -no-reboot
}

# Запуск RISC-V64 (если есть ядро)
run_riscv64() {
    print_header "Starting MyOS on RISC-V64"

    KERNEL="./kernel/build/kernel-riscv64.bin"
    if [ ! -f "$KERNEL" ]; then
        print_error "$KERNEL not found. Please build kernel first:"
        echo "  cd kernel && make ARCH=riscv64"
        exit 1
    fi

    print_info "Machine: QEMU virt"
    print_info "CPU: Generic RISC-V64"
    print_info "RAM: 512 MB"
    echo ""

    check_qemu riscv64 || { print_error "QEMU riscv64 not available"; exit 1; }

    qemu-system-riscv64 \
        -M virt \
        -m 512 \
        -kernel "$KERNEL" \
        -serial stdio \
        -display none \
        -no-reboot
}

# Главное меню
show_menu() {
    print_header "🖥️  MyOS v1.0 - QEMU Runner"
    echo ""
    echo "Select architecture to run:"
    echo ""
    print_option "1" "x86_64 (Intel/AMD) with BIOS/GRUB"
    print_option "2" "ARM64 (Cortex-A72)"
    print_option "3" "RISC-V64"
    print_option "4" "Exit"
    echo ""
}

# Обработка аргументов командной строки
case "${1:-menu}" in
    x86_64|x86-64|x64)
        run_x86_64
        ;;
    arm64|aarch64)
        run_arm64
        ;;
    riscv64|riscv)
        run_riscv64
        ;;
    menu|--menu|-m)
        show_menu
        read -p "Enter your choice (1-4): " choice
        case "$choice" in
            1) run_x86_64 ;;
            2) run_arm64 ;;
            3) run_riscv64 ;;
            4) print_info "Exiting..."; exit 0 ;;
            *) print_error "Invalid choice"; exit 1 ;;
        esac
        ;;
    help|--help|-h)
        echo "Usage: $0 [ARCH|OPTION]"
        echo ""
        echo "Available architectures:"
        echo "  x86_64, x86-64, x64   - Run on x86_64"
        echo "  arm64, aarch64        - Run on ARM64"
        echo "  riscv64, riscv        - Run on RISC-V64"
        echo ""
        echo "Options:"
        echo "  menu, --menu, -m      - Show interactive menu (default)"
        echo "  help, --help, -h      - Show this help message"
        echo ""
        ;;
    *)
        print_error "Unknown architecture: $1"
        echo "Use '$0 --help' for usage information"
        exit 1
        ;;
esac

#!/bin/bash
# run_qemu.sh - запуск MyOS на QEMU x86_64
# Использование: ./run_qemu.sh [options]
# Опции: --vnc (для графического режима вместо консоли)

ISO="./myos.iso"
DISPLAY_MODE="none"
VNC_ENABLED=false

# Парсируем аргументы
for arg in "$@"; do
    case "$arg" in
        --vnc)
            VNC_ENABLED=true
            DISPLAY_MODE="vnc=127.0.0.1:5900"
            ;;
        --help|-h)
            echo "MyOS QEMU Runner"
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --vnc    Enable VNC display (instead of console)"
            echo "  --help   Show this help message"
            exit 0
            ;;
    esac
done

if [ ! -f "$ISO" ]; then
    echo "❌ Error: $ISO not found"
    echo "Please run ./build_iso.sh first"
    exit 1
fi

echo "╔════════════════════════════════════════════════════╗"
echo "║          🖥️  MyOS v1.0 - QEMU Runner 🖥️           ║"
echo "╚════════════════════════════════════════════════════╝"
echo ""
echo "📊 System Configuration:"
echo "   ├─ CPU: 2 cores"
echo "   ├─ RAM: 512 MB"
echo "   ├─ Boot: ISO ($ISO)"
echo "   ├─ Display: $DISPLAY_MODE"
echo "   └─ KVM: Auto-detect"
echo ""
echo "⚙️  Running QEMU..."
echo "   Press Ctrl+A then X to exit (console mode)"
echo ""

# Попытаемся запустить с KVM, если доступен
qemu-system-x86_64 \
    -cdrom "$ISO" \
    -m 512 \
    -smp 2 \
    -serial stdio \
    -no-reboot \
    -display "$DISPLAY_MODE" \
    -enable-kvm 2>/dev/null || {

    # Fallback без KVM
    echo "⚠️  KVM не доступен, запуск без ускорения..."
    qemu-system-x86_64 \
        -cdrom "$ISO" \
        -m 512 \
        -smp 2 \
        -serial stdio \
        -no-reboot \
        -display "$DISPLAY_MODE"
}
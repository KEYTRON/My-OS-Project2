#!/bin/bash
# Скрипт для запуска ARM64 ядра в QEMU

KERNEL="/Users/keytron46/git/GitHub_Desktop/kernel/build/kernel-arm64.bin"

if [ ! -f "$KERNEL" ]; then
    echo "❌ Error: kernel not found at $KERNEL"
    echo "Please build kernel first: cd kernel && make ARCH=arm64"
    exit 1
fi

echo ""
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║              🖥️  MyOS ARM64 QEMU Runner                        ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""
echo "📊 Configuration:"
echo "   ├─ Machine: QEMU virt (ARMv8-A)"
echo "   ├─ CPU: Cortex-A72"
echo "   ├─ RAM: 512 MB"
echo "   └─ Kernel: $KERNEL ($(du -h $KERNEL | cut -f1))"
echo ""
echo "🔍 Boot Trace (via UART):"
echo "   A - Entry to _start"
echo "   B - Stack initialized"
echo "   b - BSS clearing started"
echo "   s - BSS size loaded"
echo "   m - memzero called"
echo "   z - memzero completed"
echo "   C - kernel_main called"
echo "   X - kernel_main returned (or infinite loop)"
echo ""
echo "⌨️  Controls:"
echo "   • Ctrl+C - Terminate emulation"
echo "   • Ctrl+A then X - Exit QEMU (if using monitor)"
echo ""
echo "╔════════════════════════════════════════════════════════════════╗"
echo ""

# Kill any existing QEMU processes
pkill -9 qemu 2>/dev/null || true
pkill -9 qemu-system 2>/dev/null || true
sleep 1

# Start QEMU
qemu-system-aarch64 \
    -M virt \
    -cpu cortex-a72 \
    -m 512 \
    -kernel "$KERNEL" \
    -serial stdio \
    -monitor none \
    -display none \
    -no-reboot

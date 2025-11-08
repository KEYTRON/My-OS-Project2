#!/bin/bash
# Скрипт для отладки ARM64 ядра с более подробным выводом

KERNEL="/Users/keytron46/git/GitHub_Desktop/kernel/build/kernel-arm64.bin"

if [ ! -f "$KERNEL" ]; then
    echo "Error: kernel not found at $KERNEL"
    exit 1
fi

echo "Testing ARM64 kernel..."
echo "Kernel size: $(du -h $KERNEL | cut -f1)"
echo ""

# Запускаем с -d и другими опциями отладки
timeout 5 qemu-system-aarch64 \
    -M virt \
    -cpu cortex-a72 \
    -m 512 \
    -kernel "$KERNEL" \
    -serial mon:stdio \
    -display none \
    -d cpu_reset,int,guest_errors \
    -D /tmp/qemu_arm64.log \
    2>&1

echo ""
echo "QEMU exited or timed out"
echo ""

if [ -f /tmp/qemu_arm64.log ]; then
    echo "First 50 lines of QEMU debug log:"
    head -50 /tmp/qemu_arm64.log
fi

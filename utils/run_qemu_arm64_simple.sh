#!/bin/bash
# Простой скрипт для запуска ARM64 ядра

KERNEL="/Users/keytron46/git/GitHub_Desktop/kernel/build/kernel-arm64.bin"

echo "Starting ARM64 kernel..."
qemu-system-aarch64 \
    -M virt \
    -cpu cortex-a72 \
    -m 512 \
    -kernel "$KERNEL" \
    -serial stdio \
    -display none \
    -nographic \
    -drive if=none,id=blk,file=/dev/null \
    -device virtio-blk-device,drive=blk

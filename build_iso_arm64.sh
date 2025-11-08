#!/bin/bash
# Скрипт для создания ARM64 Live CD образа

set -e

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║          Building ARM64 MyOS Live CD v1.0                      ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

# Проверяем наличие ядра
KERNEL="kernel/build/kernel-arm64.bin"
if [ ! -f "$KERNEL" ]; then
    echo "❌ Error: ARM64 kernel not found at $KERNEL"
    echo "Please build kernel first: cd kernel && make ARCH=arm64"
    exit 1
fi

KERNEL_SIZE=$(du -h "$KERNEL" | cut -f1)
echo "✓ Found ARM64 kernel: $KERNEL ($KERNEL_SIZE)"
echo ""

# Для ARM64 можем использовать несколько подходов:
# 1. Простой бинарный образ (для запуска в QEMU)
# 2. Образ диска с разделами (более продвинутый)

# Сейчас создадим простой вариант - просто бинарное ядро
OUTPUT="myos-arm64.bin"

echo "📝 Creating ARM64 binary image..."
cp "$KERNEL" "$OUTPUT"

SIZE=$(du -h "$OUTPUT" | cut -f1)
echo "✓ Created: $OUTPUT ($SIZE)"
echo ""

echo "🚀 To run in QEMU:"
echo "   qemu-system-aarch64 -M virt -cpu cortex-a72 -m 512 -kernel $OUTPUT -serial stdio -monitor none -display none"
echo ""
echo "   Or use the runner script:"
echo "   ./utils/run_qemu_arm64.sh"
echo ""

echo "╔════════════════════════════════════════════════════════════════╗"
echo "✓ Build complete!"
echo "╚════════════════════════════════════════════════════════════════╝"

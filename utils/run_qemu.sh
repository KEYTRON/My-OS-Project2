#!/bin/bash
# run_qemu.sh — запускает ISO в QEMU

ISO=myos.iso

if [ ! -f $ISO ]; then
    echo "Не найден ISO: $ISO. Сначала запусти build_iso.sh"
    exit 1
fi

qemu-system-x86_64 \
    -cdrom $ISO \
    -m 512M \
    -serial stdio \
    -no-reboot \
    -display none \
    -enable-kvm \
    -smp 2
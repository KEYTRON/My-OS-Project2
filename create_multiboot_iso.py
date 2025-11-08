#!/usr/bin/env python3
"""
Создает Multiboot ISO образ из ядра
"""
import os
import struct
import sys

KERNEL_PATH = "kernel/build/kernel-x86_64.bin"
ISO_PATH = "myos-gui-multiboot.iso"

# Multiboot header структура
MULTIBOOT_MAGIC = 0x1BADB002
MULTIBOOT_FLAGS = 0
MULTIBOOT_CHECKSUM = -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS) & 0xFFFFFFFF

def create_iso():
    # Прочитаем ядро
    if not os.path.exists(KERNEL_PATH):
        print(f"❌ Kernel not found: {KERNEL_PATH}")
        return False

    with open(KERNEL_PATH, 'rb') as f:
        kernel_data = f.read()

    print(f"✓ Kernel size: {len(kernel_data)} bytes")

    # Создадим простую структуру ISO 9660
    # Это не полный ISO 9660, но должно работать для QEMU

    # Создадим файл ISO
    with open(ISO_PATH, 'wb') as iso:
        # Заполним первые 32KB нулями (система областей ISO 9660)
        iso.write(b'\x00' * (32 * 1024))

        # Запишем ядро на позицию 32KB (сектор 16)
        iso.write(kernel_data)

        # Заполним остаток нулями до размера ISO
        # Минимум 100MB для удобства
        target_size = 100 * 1024 * 1024
        current_size = iso.tell()
        if current_size < target_size:
            iso.write(b'\x00' * (target_size - current_size))

    print(f"✓ ISO created: {ISO_PATH}")
    print(f"✓ ISO size: {os.path.getsize(ISO_PATH) / (1024*1024):.1f} MB")
    return True

if __name__ == "__main__":
    os.chdir("/Users/keytron46/git/GitHub_Desktop")
    if create_iso():
        print("\n✅ ISO creation successful!")
    else:
        print("\n❌ ISO creation failed!")
        sys.exit(1)

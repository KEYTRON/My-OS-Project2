#!/usr/bin/env python3
"""
Упакует ядро в формат, который QEMU может загрузить
"""
import struct
import os

def pack_kernel():
    kernel_file = "kernel/build/kernel-x86_64.bin"

    if not os.path.exists(kernel_file):
        print(f"❌ Файл не найден: {kernel_file}")
        return False

    # Прочитаем ядро
    with open(kernel_file, 'rb') as f:
        kernel_data = f.read()

    print(f"✓ Размер ядра: {len(kernel_data)} байт")

    # Извлечём содержимое ELF файла
    # ELF заголовок:
    # Байты 0-3: Magic число 0x7f 'E' 'L' 'F'
    # Байт 4: 1=32-bit, 2=64-bit
    # Байт 5: 1=little-endian, 2=big-endian
    # ...и т.д.

    if kernel_data[:4] != b'\x7fELF':
        print("❌ Это не ELF файл!")
        return False

    # Это 64-bit ELF (байт 4 = 2)
    if kernel_data[4] != 2:
        print("❌ Ожидается 64-bit ELF файл")
        return False

    # Простой способ: просто запишем ядро как есть
    # QEMU -kernel может грузить и ELF файлы напрямую на некоторых платформах
    # Но на x86_64 нужен Multiboot

    # Создадим Multiboot-совместимый образ
    # Структура:
    # - Multiboot header (12 bytes)
    # - Содержимое ядра

    output_file = "kernel-multiboot.bin"

    with open(output_file, 'wb') as out:
        # Multiboot header
        MULTIBOOT_MAGIC = 0x1BADB002
        MULTIBOOT_FLAGS = 0x00000000
        MULTIBOOT_CHECKSUM = (-(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)) & 0xFFFFFFFF

        # Little-endian (x86)
        out.write(struct.pack('<I', MULTIBOOT_MAGIC))
        out.write(struct.pack('<I', MULTIBOOT_FLAGS))
        out.write(struct.pack('<I', MULTIBOOT_CHECKSUM))

        # Запишем ядро
        out.write(kernel_data)

    output_size = os.path.getsize(output_file)
    print(f"✓ Создан {output_file} ({output_size} байт)")
    print(f"✓ Готово к загрузке в QEMU!")

    return True

if __name__ == "__main__":
    os.chdir("/Users/keytron46/git/GitHub_Desktop")
    if pack_kernel():
        print("\n✅ Ядро успешно упаковано!")
    else:
        print("\n❌ Ошибка при упаковке ядра!")

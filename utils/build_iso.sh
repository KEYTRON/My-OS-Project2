#!/bin/bash
# build_iso.sh — собирает ISO-образ под BIOS (не UEFI)

# 1) Собираем ядро
echo "Сборка ядра..."
cd ../kernel
make clean
make all
if [ $? -ne 0 ]; then
    echo "Ошибка сборки ядра!"
    exit 1
fi

# 2) Подготавливаем структуру ISO
echo "Подготовка структуры ISO..."
ISO_ROOT=iso_root
rm -rf $ISO_ROOT
mkdir -p $ISO_ROOT/boot/grub

# 3) Копируем ядро и конфиг
cp build/kernel.bin $ISO_ROOT/boot/kernel.bin
cp ../boot/grub/grub.cfg $ISO_ROOT/boot/grub/grub.cfg

# 4) Собираем ISO (BIOS-режим)
echo "Создание ISO..."
# Попробуем использовать x86_64-elf-grub-mkrescue
if command -v x86_64-elf-grub-mkrescue &> /dev/null; then
    echo "Используем x86_64-elf-grub-mkrescue..."
    x86_64-elf-grub-mkrescue -o myos.iso $ISO_ROOT
elif command -v grub-mkrescue &> /dev/null; then
    echo "Используем grub-mkrescue..."
    grub-mkrescue -o myos.iso $ISO_ROOT
else
    echo "grub-mkrescue не найден, пытаемся через xorriso..."
    # Сначала нужно создать загрузочный образ GRUB
    if [ ! -f /opt/homebrew/bin/x86_64-elf-grub-mkrescue ]; then
        echo "Ошибка: x86_64-elf-grub-mkrescue не найден в /opt/homebrew/bin/"
        echo "Попробуйте установить: brew install x86_64-elf-grub"
        exit 1
    fi
    
    # Используем полный путь к x86_64-elf-grub-mkrescue
    /opt/homebrew/bin/x86_64-elf-grub-mkrescue -o myos.iso $ISO_ROOT
fi

if [ $? -ne 0 ]; then
    echo "Ошибка: не удалось создать ISO."
    exit 1
fi

echo "ISO успешно создан: myos.iso"
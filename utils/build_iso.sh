#!/bin/bash
# build_iso.sh — собирает ISO‑образ под BIOS (не UEFI)

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
cp ../boot/grub.cfg $ISO_ROOT/boot/grub/grub.cfg

# 4) Собираем ISO (BIOS‑режим)
echo "Создание ISO..."
grub-mkrescue -o myos.iso $ISO_ROOT  2> /dev/null
if [ $? -ne 0 ]; then
    echo "grub-mkrescue не получилось, пытаемся через xorriso..."
    xorriso -as mkisofs \
        -R -J -b boot/grub/i386-pc/eltorito.img \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        -o myos.iso $ISO_ROOT
    if [ $? -ne 0 ]; then
        echo "Ошибка: не удалось создать ISO ни через grub-mkrescue, ни через xorriso."
        exit 1
    fi
fi

echo "ISO успешно создан: myos.iso"
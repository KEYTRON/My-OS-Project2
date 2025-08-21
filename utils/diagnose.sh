#!/bin/bash
# diagnose.sh - диагностика проблем с загрузкой

echo "=== MyOS Diagnostic Tool ==="
echo ""

# 1. Проверяем наличие необходимых файлов
echo "1. Проверка файлов..."
if [ -f "myos.iso" ]; then
    echo "✓ ISO образ найден: $(ls -lh myos.iso)"
else
    echo "✗ ISO образ не найден"
    exit 1
fi

if [ -f "../kernel/build/kernel.bin" ]; then
    echo "✓ Ядро найдено: $(ls -lh ../kernel/build/kernel.bin)"
else
    echo "✗ Ядро не найдено"
    exit 1
fi

# 2. Проверяем структуру ISO
echo ""
echo "2. Проверка структуры ISO..."
mkdir -p /tmp/iso_check
hdiutil attach -readonly -mountpoint /tmp/iso_check myos.iso > /dev/null 2>&1

if [ $? -eq 0 ]; then
    echo "✓ ISO успешно смонтирован"
    
    if [ -f "/tmp/iso_check/boot/kernel.bin" ]; then
        echo "✓ Ядро в ISO: $(ls -lh /tmp/iso_check/boot/kernel.bin)"
    else
        echo "✗ Ядро не найдено в ISO"
    fi
    
    if [ -f "/tmp/iso_check/boot/grub/grub.cfg" ]; then
        echo "✓ Конфигурация GRUB найдена"
        echo "  Содержимое grub.cfg:"
        cat /tmp/iso_check/boot/grub/grub.cfg
    else
        echo "✗ Конфигурация GRUB не найдена"
    fi
    
    hdiutil detach /tmp/iso_check > /dev/null 2>&1
else
    echo "✗ Не удалось смонтировать ISO"
fi

# 3. Проверяем ELF файл
echo ""
echo "3. Проверка ELF файла..."
if command -v x86_64-elf-objdump > /dev/null 2>&1; then
    echo "✓ x86_64-elf-objdump доступен"
    
    # Проверяем точку входа
    ENTRY=$(x86_64-elf-objdump -f ../kernel/build/kernel.bin | grep "start address")
    echo "  $ENTRY"
    
    # Проверяем символы
    SYMBOLS=$(x86_64-elf-objdump -t ../kernel/build/kernel.bin | grep -E "_start|kernel_main" | head -5)
    echo "  Основные символы:"
    echo "$SYMBOLS"
else
    echo "✗ x86_64-elf-objdump недоступен"
fi

# 4. Проверяем Multiboot header
echo ""
echo "4. Проверка Multiboot header..."
if command -v x86_64-elf-objdump > /dev/null 2>&1; then
    MULTIBOOT=$(x86_64-elf-objdump -s -j .text ../kernel/build/kernel.bin | grep -A 1 "Contents of section .text" | tail -1)
    echo "  Первые байты .text секции:"
    echo "$MULTIBOOT"
    
    # Проверяем magic number (little-endian: 02b0ad1b)
    MAGIC=$(echo "$MULTIBOOT" | grep -o "02b0ad1b\|1badb002")
    if [ -n "$MAGIC" ]; then
        echo "✓ Multiboot magic number найден: $MAGIC"
        echo "  Это соответствует 0x1BADB002 в little-endian формате"
    else
        echo "✗ Multiboot magic number не найден"
        echo "  Ожидается: 02b0ad1b (little-endian для 0x1BADB002)"
    fi
fi

echo ""
echo "=== Диагностика завершена ==="

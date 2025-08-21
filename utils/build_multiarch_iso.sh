#!/bin/bash
# build_multiarch_iso.sh — собирает ISO-образы для всех поддерживаемых архитектур

echo "=== MyOS Multi-Architecture ISO Builder ==="
echo ""

# Проверяем наличие необходимых инструментов
check_tools() {
    local missing_tools=()
    
    if ! command -v xorriso &> /dev/null; then
        missing_tools+=("xorriso")
    fi
    
    if ! command -v qemu-system-x86_64 &> /dev/null; then
        missing_tools+=("qemu-system-x86_64")
    fi
    
    if ! command -v qemu-system-aarch64 &> /dev/null; then
        missing_tools+=("qemu-system-aarch64")
    fi
    
    if ! command -v qemu-system-riscv64 &> /dev/null; then
        missing_tools+=("qemu-system-riscv64")
    fi
    
    if [ ${#missing_tools[@]} -ne 0 ]; then
        echo "✗ Отсутствуют необходимые инструменты: ${missing_tools[*]}"
        echo "Установите их с помощью: ./install_cross_compilers.sh"
        exit 1
    fi
    
    echo "✓ Все необходимые инструменты найдены"
}

# Собираем ядро для указанной архитектуры
build_kernel() {
    local arch=$1
    echo "Сборка ядра для архитектуры $arch..."
    
    cd kernel
    make clean
    make ARCH=$arch all
    
    if [ $? -ne 0 ]; then
        echo "✗ Ошибка сборки ядра для $arch"
        cd ..
        return 1
    fi
    
    echo "✓ Ядро для $arch собрано успешно"
    cd ..
}

# Создаем ISO для указанной архитектуры
create_iso() {
    local arch=$1
    local kernel_name=""
    local iso_name=""
    
    case $arch in
        "x86_64")
            kernel_name="kernel-x86_64.bin"
            iso_name="myos-x86_64.iso"
            ;;
        "arm64")
            kernel_name="kernel-arm64.bin"
            iso_name="myos-arm64.iso"
            ;;
        "riscv64")
            kernel_name="kernel-riscv64.bin"
            iso_name="myos-riscv64.iso"
            ;;
        *)
            echo "✗ Неподдерживаемая архитектура: $arch"
            return 1
            ;;
    esac
    
    echo "Создание ISO для $arch..."
    
    # Подготавливаем структуру ISO
    ISO_ROOT="iso_root_$arch"
    rm -rf $ISO_ROOT
    mkdir -p $ISO_ROOT/boot/grub
    
    # Копируем ядро
    echo "Проверяем файл: kernel/build/$kernel_name"
    if [ -f "kernel/build/$kernel_name" ]; then
        echo "Ядро найдено, копируем..."
        cp "kernel/build/$kernel_name" "$ISO_ROOT/boot/kernel.bin"
    else
        echo "✗ Ядро $kernel_name не найдено"
        echo "Текущая директория: $(pwd)"
        echo "Содержимое kernel/: $(ls -la kernel/ 2>/dev/null || echo 'Папка kernel не найдена')"
        return 1
    fi
    
    # Копируем конфигурацию GRUB
    cp "boot/grub/grub.cfg" "$ISO_ROOT/boot/grub/grub.cfg"
    
    # Создаем ISO
    echo "Создание ISO образа..."
    
    # Пробуем использовать grub-mkrescue
    if command -v grub-mkrescue >/dev/null 2>&1; then
        echo "Используем grub-mkrescue..."
        grub-mkrescue -o "$iso_name" "$ISO_ROOT"
    elif command -v x86_64-elf-grub-mkrescue >/dev/null 2>&1; then
        echo "Используем x86_64-elf-grub-mkrescue..."
        x86_64-elf-grub-mkrescue -o "$iso_name" "$ISO_ROOT"
    else
        echo "Пробуем использовать xorriso..."
        xorriso -as mkisofs \
            -R -J -b boot/grub/i386-pc/eltorito.img \
            -no-emul-boot -boot-load-size 4 -boot-info-table \
            -o "$iso_name" "$ISO_ROOT"
    fi
    
    if [ $? -eq 0 ]; then
        echo "✓ ISO для $arch создан: $iso_name"
        rm -rf "$ISO_ROOT"
    else
        echo "✗ Ошибка создания ISO для $arch"
        rm -rf "$ISO_ROOT"
        return 1
    fi
}

# Тестируем ISO в QEMU
test_iso() {
    local arch=$1
    local iso_name=""
    local qemu_cmd=""
    
    case $arch in
        "x86_64")
            iso_name="myos-x86_64.iso"
            qemu_cmd="qemu-system-x86_64 -cdrom $iso_name -m 512M -serial stdio -no-reboot -display none"
            ;;
        "arm64")
            iso_name="myos-arm64.iso"
            qemu_cmd="qemu-system-aarch64 -cdrom $iso_name -m 512M -serial stdio -no-reboot -display none -machine virt"
            ;;
        "riscv64")
            iso_name="myos-riscv64.iso"
            qemu_cmd="qemu-system-riscv64 -cdrom $iso_name -m 512M -serial stdio -no-reboot -display none -machine virt"
            ;;
        *)
            echo "✗ Неподдерживаемая архитектура для тестирования: $arch"
            return 1
            ;;
    esac
    
    if [ -f "$iso_name" ]; then
        echo "Тестирование ISO для $arch в QEMU..."
        echo "Команда: $qemu_cmd"
        echo "Нажмите Ctrl+C для выхода из QEMU"
        echo ""
        
        # Запускаем QEMU в фоне
        eval "$qemu_cmd" &
        QEMU_PID=$!
        
        # Ждем немного и проверяем, что QEMU запустился
        sleep 2
        if kill -0 $QEMU_PID 2>/dev/null; then
            echo "QEMU запущен (PID: $QEMU_PID)"
            echo "Для остановки: kill $QEMU_PID"
        else
            echo "✗ Ошибка запуска QEMU для $arch"
        fi
    else
        echo "✗ ISO файл $iso_name не найден"
    fi
}

# Основная функция
main() {
    local architectures=("x86_64" "arm64" "riscv64")
    local build_all=false
    local test_mode=false
    
    # Парсим аргументы
    while [[ $# -gt 0 ]]; do
        case $1 in
            -a|--all)
                build_all=true
                shift
                ;;
            -t|--test)
                test_mode=true
                shift
                ;;
            -h|--help)
                echo "Использование: $0 [опции] [архитектура]"
                echo ""
                echo "Опции:"
                echo "  -a, --all     Собрать для всех архитектур"
                echo "  -t, --test    Протестировать ISO в QEMU"
                echo "  -h, --help    Показать эту справку"
                echo ""
                echo "Архитектуры: x86_64, arm64, riscv64"
                echo ""
                echo "Примеры:"
                echo "  $0 x86_64              # Собрать только для x86_64"
                echo "  $0 -a                  # Собрать для всех архитектур"
                echo "  $0 -t x86_64           # Собрать и протестировать x86_64"
                echo "  $0 -a -t               # Собрать для всех и протестировать"
                exit 0
                ;;
            x86_64|arm64|riscv64)
                if [ "$build_all" = false ]; then
                    architectures=("$1")
                fi
                shift
                ;;
            *)
                echo "✗ Неизвестный аргумент: $1"
                echo "Используйте -h для справки"
                exit 1
                ;;
        esac
    done
    
    # Проверяем инструменты
    check_tools
    
    # Собираем ядра
    for arch in "${architectures[@]}"; do
        echo ""
        echo "=== Архитектура: $arch ==="
        
        if build_kernel "$arch"; then
            if create_iso "$arch"; then
                echo "✓ $arch: успешно"
                
                # Тестируем если нужно
                if [ "$test_mode" = true ]; then
                    test_iso "$arch"
                fi
            else
                echo "✗ $arch: ошибка создания ISO"
            fi
        else
            echo "✗ $arch: ошибка сборки ядра"
        fi
    done
    
    echo ""
    echo "=== Сборка завершена ==="
    
    # Показываем созданные файлы
    echo "Созданные ISO файлы:"
    for arch in "${architectures[@]}"; do
        case $arch in
            "x86_64")
                if [ -f "myos-x86_64.iso" ]; then
                    echo "  ✓ myos-x86_64.iso ($(ls -lh myos-x86_64.iso | awk '{print $5}'))"
                fi
                ;;
            "arm64")
                if [ -f "myos-arm64.iso" ]; then
                    echo "  ✓ myos-arm64.iso ($(ls -lh myos-arm64.iso | awk '{print $5}'))"
                fi
                ;;
            "riscv64")
                if [ -f "myos-riscv64.iso" ]; then
                    echo "  ✓ myos-riscv64.iso ($(ls -lh myos-riscv64.iso | awk '{print $5}'))"
                fi
                ;;
        esac
    done
}

# Запускаем основную функцию
main "$@"

#!/bin/bash
# install_tools.sh — установка инструментов для разработки ОС

echo "Установка инструментов для разработки ОС..."

# Проверяем наличие Homebrew
if ! command -v brew &> /dev/null; then
    echo "Ошибка: Homebrew не установлен. Установите Homebrew сначала."
    exit 1
fi

# Устанавливаем базовые инструменты
echo "Установка NASM..."
brew install nasm

echo "Установка QEMU..."
brew install qemu

echo "Установка xorriso (для создания ISO)..."
brew install xorriso

echo "Установка grub (для создания загрузочного ISO)..."
brew install grub

echo "Установка зависимостей для кросс-компилятора..."
brew install gmp libmpc mpfr isl zlib

echo "Скачивание и установка кросс-компилятора..."
cd /tmp

# Скачиваем binutils
echo "Скачивание binutils..."
wget https://ftp.gnu.org/gnu/binutils/binutils-2.40.tar.gz
tar xf binutils-2.40.tar.gz
cd binutils-2.40

echo "Сборка binutils..."
./configure --target=x86_64-elf --with-sysroot --disable-nls --disable-werror
make -j$(nproc)
sudo make install

cd ..

# Скачиваем gcc
echo "Скачивание GCC..."
wget https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.gz
tar xf gcc-13.2.0.tar.gz
cd gcc-13.2.0

echo "Сборка GCC..."
./configure --target=x86_64-elf --disable-nls --disable-libssp --without-isl --without-cloog \
    --disable-shared --disable-threads --disable-libmudflap --disable-libquadmath \
    --disable-libstdcxx --disable-libgomp
make all-gcc -j$(nproc)
sudo make install-gcc

cd ..

# Очистка
rm -rf binutils-2.40* gcc-13.2.0*

echo "Проверка установки..."
if command -v x86_64-elf-gcc &> /dev/null; then
    echo "✓ x86_64-elf-gcc установлен"
    x86_64-elf-gcc --version
else
    echo "✗ x86_64-elf-gcc не найден"
fi

if command -v x86_64-elf-ld &> /dev/null; then
    echo "✓ x86_64-elf-ld установлен"
    x86_64-elf-ld --version
else
    echo "✗ x86_64-elf-ld не найден"
fi

if command -v nasm &> /dev/null; then
    echo "✓ NASM установлен"
    nasm --version
else
    echo "✗ NASM не найден"
fi

if command -v qemu-system-x86_64 &> /dev/null; then
    echo "✓ QEMU установлен"
    qemu-system-x86_64 --version
else
    echo "✗ QEMU не найден"
fi

echo "Установка завершена!"

# Дизайн и архитектура ядра MyOS



## 1. Multiboot и загрузка
- Используем стандартный загрузчик GRUB (Multiboot‑2).  
- `entry.S` содержит Multiboot‑заголовок (magic, flags, checksum), после чего начинается ассемблерный код бута.

### 1.1. Переключение в 64‑бит (Long Mode)
1. Переходим в Protected Mode (CR0).  
2. Включаем PAE (CR4).  
3. Включаем LME (MSR EFER).  
4. Делаем far jump в 64‑битный кодовый сегмент.  
5. Устанавливаем сегменты данных (DS, ES, FS, GS, SS).  
6. Вызываем функцию `kernel_main`.

## 2. GDT
- Структура GDT:
  - NULL‑сегмент (индекс 0).  
  - Кодовый сегмент ядра (индекс 1).  
  - Данные сегмент ядра (индекс 2).  
  - Кодовый сегмент пользователя (индекс 3).  
  - Данные сегмент пользователя (индекс 4).  
  - TSS (индекс 5, зарезервирован, пока пустой).  
- После загрузки GDT мы можем переключаться между кольцами (DPL) для пользователей, если написали планировщик и менеджер процессов.

## 3. IDT
- Таблица из 256 дескрипторов.  
- Дескрипторы 0..31 — исключения CPU (DIV/0, GPF, PF и т. д.).  
- Дескрипторы 32..47 — IRQ (PC‑шина).  
- Остальные зарезервированы.  
- Для перехода из ассемблера в C у нас есть «заглушки» (ISR stubs), которые сохраняют регистры, вызывают `isr_handler`/`irq_handler`, потом восстанавливают регистры и делают `iretq`.

## 4. Пейджинг
- Три уровня: PML4, PDPT, PD.  
- Отображение 1 : 1 для первых 1 GiB памяти (каждая запись PD – 2 MiB страница).  
- Сохраняем `PML4` в CR3 и включаем бит PG в CR0.  
- После этого virtual = physical (до 1 GiB).  
- Далее можно расширять: аллоцировать новые таблицы PDPT[1], PDPT[2]… и т. д.

## 5. Драйверы
### 5.1. VGA
- Текстовый режим 80×25, буфер по адресу `0xB8000`.  
- Функции `vga_init`, `vga_putc_color`, `vga_write_string`.  

### 5.2. Serial (COM1)
- Базовая инициализация: 38400 baud, 8N1.  
- Функции `serial_init`, `serial_write_char`, `serial_write_string`.  

### 5.3. Keyboard (PS/2)
- IRQ1.  
- При вводе символа: считываем scan code из порта `0x60`;  
- Преобразуем в ASCII через keymap;  
- Выводим на VGA (цвет зелёный).  

## 6. Библиотека lib
- `memcpy`, `memset` (примитивные).  
- `strlen`, `strcpy`, `strcmp`.  
- Упрощённый `printf`, поддерживающий `%s`, `%d`, `%u`, `%x`, `%c`, `%%`.  
- `printf` может выводить либо на VGA, либо на COM1 (устанавливается макросом).

## 7. Makefile и сборка
- Используем `x86_64-elf-gcc`, `x86_64-elf-ld`, `nasm`.  
- Компиляция всех `.c` и `.S` → `.o`, затем `ld -T linker.ld` → `kernel.bin`.  
- Скрипт `build_iso.sh` добавляет ядро и `grub.cfg` в ISO.  
- Скрипт `run_qemu.sh` запускает ISO в QEMU.

## 8. Планы на будущее
1. **Менеджер памяти:**  
   - Реализовать `kmalloc`, `kfree`.  
   - Организовать `kmalloc` через простую структуру «свободных страниц» (фреймов).  
   - Поддержка `virtio`‑дисков, файловой системы (FAT, ext2 и т. д.).

2. **Планировщик:**  
   - Простейший Round‑Robin.  
   - Структура TCB (Task Control Block), готовая запуска RISC набора инструкций.  
   - Миграция между ядрами (SMP).

3. **Поддержка других архитектур:**  
   - Добавить папку `arch/arm64`, где описать свой `entry.S` для ARM64 (UEFI или Bare Metal).  
   - Добавить `arch/riscv64` с аналогичным бутстрэпом.  
   - В Makefile сделать условный выбор архитектуры (например, `make ARCH=arm64`).

4. **Сетевой стек:**  
   - Реализовать драйверы для сетевой карты (Ethernet, VirtIO).  
   - Добавить простую TCP/IP стек (LWIP, uIP).

5. **Файловая система:**  
   - Добавить драйвер SATA/SCSI.  
   - Реализовать VFS, потом FAT32 или ext2.

6. **Графический интерфейс:**  
   - Добавить драйвер Framebuffer или VESA VBE.  
   - Написать простое окно + рисование текста.

7. **Безопасность:**  
   - Поддержка UEFI Secure Boot (наследует сертификаты).  
   - ASLR (Address Space Layout Randomization).  
   - Аппаратная виртуализация (VT‑х).  

8. **Дополнительно:**  
   - Интеграция в HomeAssistant для управления самим ядром? (шутка)  
   - Развивать ядро как модульную «микроядровую» архитектуру: отдельный процесс для драйверов, IPC, message passing.  

---

## 6. Пример сборки и запуска

1. **Настройка кросс‑компилятора** (под macOS):  
   - В `/utils/cross_compile_setup.md` добавить:
     ```markdown
     # Как собрать кросс‑компилятор x86_64-elf‑gcc под macOS

     1. Установим зависимости (через Homebrew):
        ```
        brew install gmp libmpc mpfr isl zlib
        ```
     2. Скачиваем binutils:
        ```
        wget https://ftp.gnu.org/gnu/binutils/binutils-2.40.tar.gz
        tar xf binutils-2.40.tar.gz
        cd binutils-2.40
        ./configure --target=x86_64-elf --with-sysroot --disable-nls --disable-werror
        make
        sudo make install
        ```
     3. Скачиваем gcc:
        ```
        wget https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.gz
        tar xf gcc-13.2.0.tar.gz
        cd gcc-13.2.0
        ./configure --target=x86_64-elf --disable-nls --disable-libssp --without-isl --without-cloog \
            --disable-shared --disable-threads --disable-libmudflap --disable-libquadmath \
            --disable-libstdcxx --disable-libgomp
        make all-gcc
        sudo make install-gcc
        ```
     4. Проверяем:
        ```
        x86_64-elf-gcc --version
        x86_64-elf-ld --version
        ```
     ```
   - После этого кросс‑компилятор будет доступен.

2. **Собираем ядро и ISO**:
   ```bash
   cd my-os/utils
   ./build_iso.sh
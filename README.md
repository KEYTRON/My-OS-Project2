# My OS Project 2

Проект нацелен на создание полноценной мультиархитектурной операционной системы с нуля. Включает загрузчик, ядро и другие необходимые компоненты для работы на различных архитектурах процессоров.

## 🧬 Языки ядра

Ядро собрано из четырёх языков (x86_64):

| Язык | Что делает | Где |
|------|------------|-----|
| **ASM** (nasm) | multiboot-заголовок, переход в long mode, identity map 4 GiB, заглушки прерываний | `kernel/arch/x86_64/entry.S`, `isr_stubs.S`, `gdt.S` |
| **C** | драйверы (VGA, COM1, PS/2 клавиатура, PIT-таймер), GDT/IDT, графика, kmain | `kernel/drivers/`, `kernel/arch/x86_64/`, `kernel/kmain.c` |
| **C++** | куча ядра: free-list аллокатор `kmalloc`/`kfree`/`krealloc` + operator new/delete | `kernel/lib/cxx/kheap.cpp` |
| **Rust** | интерактивная оболочка `myos>` (no_std + alloc поверх C++ кучи) | `kernel/rust/src/lib.rs` |

Что умеет ядро после загрузки:

- Полная инициализация: GDT, IDT (256 векторов, PIC remap), пейджинг, куча 16 MiB, PIT на 100 Гц, клавиатура
- Графический рабочий стол 1024×768×32 (framebuffer от GRUB через multiboot)
- Rust-оболочка на VGA-консоли и serial: `help`, `about`, `langs`, `mem`, `alloc`, `uptime`, `echo`, `clear`, `reboot`, `halt`

Быстрый старт (нужны `gcc`, `g++`, `nasm`, `grub-mkrescue`, `xorriso`, `qemu`, Rust с таргетом `x86_64-unknown-none`):

```bash
rustup target add x86_64-unknown-none
./utils/build_iso.sh
qemu-system-x86_64 -cdrom kernel/myos.iso -m 256 -serial stdio
```

## 🏗️ Поддерживаемые архитектуры

- **x86_64** - 64-битные процессоры Intel/AMD (основная поддержка)
- **ARM64** - 64-битные ARM процессоры (Raspberry Pi 4, Apple Silicon, серверы)
- **RISC-V64** - 64-битные RISC-V процессоры (эмерджинг архитектура)

## 📁 Структура проекта

```
My-OS-Project2/
├── boot/grub/grub.cfg          # Конфигурация GRUB загрузчика
├── kernel/
│   ├── kmain.c                  # Основной код ядра
│   ├── arch/                    # Архитектурно-зависимый код
│   │   ├── x86_64/             # x86_64 специфичный код
│   │   │   ├── entry.S         # Точка входа (Multiboot)
│   │   │   ├── gdt.c/h         # Global Descriptor Table
│   │   │   ├── idt.c/h         # Interrupt Descriptor Table
│   │   │   ├── isr.c/h         # Interrupt Service Routines
│   │   │   └── paging.c/h      # Управление памятью
│   │   ├── arm64/              # ARM64 специфичный код
│   │   │   ├── entry.S         # Точка входа (ARM64)
│   │   │   └── arch.h          # ARM64 специфичные функции
│   │   └── riscv64/            # RISC-V64 специфичный код
│   │       ├── entry.S         # Точка входа (RISC-V64)
│   │       └── arch.h          # RISC-V64 специфичные функции
│   ├── drivers/                 # Драйверы устройств
│   │   ├── vga.c/h             # Видеоадаптер (текстовый режим)
│   │   ├── serial.c/h          # Последовательный порт (COM1)
│   │   └── keyboard.c/h        # Клавиатура (PS/2)
│   ├── include/                 # Заголовочные файлы
│   │   ├── arch.h              # Архитектурно-независимый интерфейс
│   │   ├── boot.h              # Загрузочные константы
│   │   ├── common.h            # Общие типы и константы
│   │   ├── interrupts.h        # Интерфейс прерываний
│   │   └── memory.h            # Управление памятью
│   ├── lib/                     # Библиотечные функции
│   │   ├── printf.c/h          # Форматированный вывод
│   │   └── string.c/h          # Строковые функции
│   ├── linker.ld                # Линковочный скрипт (x86_64)
│   ├── arch/x86_64/linker.ld   # Линковочный скрипт (x86_64)
│   ├── arch/arm64/linker.ld    # Линковочный скрипт (ARM64)
│   ├── arch/riscv64/linker.ld  # Линковочный скрипт (RISC-V64)
│   └── Makefile                 # Система сборки
├── utils/                       # Утилиты сборки и тестирования
│   ├── build_iso.sh            # Сборка ISO для x86_64
│   ├── build_multiarch_iso.sh  # Сборка ISO для всех архитектур
│   ├── install_cross_compilers.sh # Установка кросс-компиляторов
│   ├── run_qemu.sh             # Запуск в QEMU
│   └── diagnose.sh             # Диагностика проблем
├── test/                        # Тесты
│   ├── test_kernel.c           # Тест компонентов ядра
│   ├── test_memory.c           # Тест функций памяти
│   └── Makefile                # Сборка тестов
├── LICENSE                      # Лицензия проекта (MIT)
└── README.md                    # Описание проекта
```

## 🚀 Сборка и запуск

### Требования

#### Для x86_64:
- `x86_64-elf-gcc` (кросс-компилятор)
- `nasm` (ассемблер)
- `grub-mkrescue` или `xorriso` (для создания ISO)
- `qemu-system-x86_64` (для эмуляции)

#### Для ARM64:
- `aarch64-elf-gcc` (кросс-компилятор)
- `qemu-system-aarch64` (для эмуляции)

#### Для RISC-V64:
- `riscv64-elf-gcc` (кросс-компилятор)
- `qemu-system-riscv64` (для эмуляции)

### Установка инструментов

```bash
# Автоматическая установка всех необходимых инструментов
cd utils
./install_cross_compilers.sh
```

### Сборка

#### Для конкретной архитектуры:
```bash
# x86_64 (по умолчанию)
cd kernel
make

# ARM64
make ARCH=arm64

# RISC-V64
make ARCH=riscv64
```

#### Для всех архитектур:
```bash
cd kernel
make all-archs
```

### Создание ISO образов

#### Для одной архитектуры:
```bash
cd utils
./build_multiarch_iso.sh x86_64    # x86_64
./build_multiarch_iso.sh arm64     # ARM64
./build_multiarch_iso.sh riscv64   # RISC-V64
```

#### Для всех архитектур:
```bash
cd utils
./build_multiarch_iso.sh -a
```

#### Сборка и тестирование:
```bash
cd utils
./build_multiarch_iso.sh -a -t     # Собрать для всех и протестировать
```

### Запуск

```bash
# x86_64
cd utils
./run_qemu.sh
```

> ℹ️ Для автоматизированных проверок предусмотрен режим быстрого выхода ядра.
> Сборка с переменной окружения `QEMU_EXIT=1` заставляет ядро
> сигнализировать QEMU о корректном завершении через `isa-debug-exit`.
> Этот режим используется в CI, чтобы убедиться, что образ действительно загружается.

```bash

# ARM64
qemu-system-aarch64 -cdrom myos-arm64.iso -m 512M -serial stdio -no-reboot -display none -machine virt

# RISC-V64
qemu-system-riscv64 -cdrom myos-riscv64.iso -m 512M -serial stdio -no-reboot -display none -machine virt
```

## ✨ Особенности

### Архитектурная независимость
- **Единый интерфейс**: `include/arch.h` предоставляет архитектурно-независимый API
- **Автоматический выбор**: Компилятор автоматически выбирает нужные функции
- **Легкое расширение**: Добавление новой архитектуры требует минимальных изменений

### x86_64 (основная поддержка)
- Поддержка Multiboot v1
- Переход в 64-битный режим (Long Mode)
- Базовая поддержка прерываний (GDT, IDT, ISR)
- Пейджинг с 1:1 отображением первых 2MB памяти

### ARM64
- Поддержка ARMv8-A архитектуры
- Инициализация для виртуальных машин (QEMU virt)
- Подготовка для GIC (Generic Interrupt Controller)

### RISC-V64
- Поддержка RV64GC архитектуры
- Инициализация для виртуальных машин (QEMU virt)
- Подготовка для PLIC (Platform-Level Interrupt Controller)

### Общие компоненты
- Драйверы: VGA (текстовый режим), Serial (COM1), Keyboard (PS/2)
- Простая библиотека функций (memcpy, memset, strlen, printf)
- Система тестирования компонентов

## 🔧 Разработка

### Добавление новой архитектуры

1. Создать папку `kernel/arch/<arch_name>/`
2. Добавить `entry.S` - точку входа
3. Добавить `arch.h` - архитектурно-специфичные функции
4. Добавить `linker.ld` - линковочный скрипт
5. Обновить `kernel/Makefile`
6. Добавить архитектуру в `include/arch.h`

### Тестирование

```bash
cd test
make test          # Запуск всех тестов
make test_kernel   # Только тесты ядра
make test_memory   # Только тесты памяти
```

## 📚 Документация

- `docs/design.md` - Дизайн и архитектура ядра
- `utils/diagnose.sh` - Диагностика проблем сборки
- Комментарии в исходном коде

## 🤝 Вклад в проект

1. Форкните репозиторий
2. Создайте ветку для новой функции
3. Внесите изменения
4. Создайте Pull Request

## 📄 Лицензия

Этот проект распространяется под лицензией MIT. См. файл `LICENSE` для подробностей.

## 🆘 Поддержка

При возникновении проблем:
1. Проверьте зависимости: `make check-deps ARCH=<architecture>`
2. Запустите диагностику: `./utils/diagnose.sh`
3. Создайте Issue с описанием проблемы

---

**MyOS** - мультиархитектурная операционная система для обучения и экспериментов! 🚀
# My OS Project

My OS Project — это экспериментальная операционная система, разрабатываемая с нуля на C/C++ и ASM. Вдохновлена macOS, Linux и современными UI-решениями.

## Структура проекта

```
/src
  /bootloader   — загрузчик (boot.asm)
  /kernel       — ядро ОС
    ├── kernel.c          — основная функция ядра
    ├── vga.c / vga.h     — вывод в текстовом режиме
    ├── memory.c / .h     — простой менеджер памяти
    ├── keyboard.c / .h   — заглушка драйвера клавиатуры
    ├── interrupts.c / .h — настройка IDT и прерываний
    ├── shell.c / .h      — простейший командный интерфейс
    ├── task.c / task.h   — базовый планировщик задач
    ├── elf.c / elf.h     — загрузчик ELF-программ
    ├── ata.c / ata.h     — драйвер чтения с диска
    └── isr.S             — asm-обработчик для IDT
/build          — артефакты сборки
/README.md      — описание проекта
```

## Как собрать

Требуется: `nasm`, `i686-elf-gcc`, `qemu-system-i386`

```bash
make run
```
 

## Планы

Все задачи и текущее состояние проекта описаны в файле [ROADMAP.md](ROADMAP.md).
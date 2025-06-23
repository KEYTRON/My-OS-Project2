#include "interrupts.h"
#include "vga.h"
// kernel.c
void kernel_main(void) {
    clear_screen();
    init_interrupts();
    const char *message = "Welcome to MyOS!";
    char *video_memory = (char*) 0xb8000;  // Адрес видеопамяти
    int i = 0;

    // Выводим сообщение на экран
    while (message[i] != '\0') {
        video_memory[i * 2] = message[i];   // Символ
        video_memory[i * 2 + 1] = 0x07;    // Цвет символа (серый на черном фоне)
        i++;
    }
    const char *second_line = "Kernel initialized.";
    i = 0;
    while (second_line[i] != '\0') {
        video_memory[160 + i * 2] = second_line[i];     // 160 байт = вторая строка
        video_memory[160 + i * 2 + 1] = 0x07;
        i++;
    }
}
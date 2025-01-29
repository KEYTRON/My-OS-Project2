// kernel.c
void kernel_main(void) {
    const char *message = "Hello, World!";
    char *video_memory = (char*) 0xb8000;  // Адрес видеопамяти
    int i = 0;

    // Выводим сообщение на экран
    while (message[i] != '\0') {
        video_memory[i * 2] = message[i];   // Символ
        video_memory[i * 2 + 1] = 0x07;    // Цвет символа (серый на черном фоне)
        i++;
    }
}
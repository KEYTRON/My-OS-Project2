// syscalls.c
#include "syscalls.h"

void sys_write(const char *str) {
    print_string(str);  // Простой системный вызов для вывода строки
}

void sys_exit() {
    print_string("Exiting...\n");  // Завершаем выполнение программы
}
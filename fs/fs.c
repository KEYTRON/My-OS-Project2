// fs.c
#include "fs.h"

#define MAX_FILES 128
static char *files[MAX_FILES];  // Простая файловая система с массивом строк

int create_file(const char *filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i] == NULL) {
            files[i] = filename;
            return 0;  // Успех
        }
    }
    return -1;  // Ошибка
}

const char *read_file(const char *filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i] != NULL && strcmp(files[i], filename) == 0) {
            return files[i];  // Возвращаем содержимое файла
        }
    }
    return NULL;  // Файл не найден
}
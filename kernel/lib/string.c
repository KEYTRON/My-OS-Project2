// string.c — реализация strlen, strcpy и strcmp
#include "string.h"

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

char* strcpy(char* dest, const char* src) {
    size_t i = 0;
    do {
        dest[i] = src[i];
    } while (src[i++] != '\0');
    return dest;
}

int strcmp(const char* a, const char* b) {
    size_t i = 0;
    while (a[i] != '\0' && b[i] != '\0' && a[i] == b[i]) {
        i++;
    }
    return (int)(a[i] - b[i]);
}
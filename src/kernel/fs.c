#include "fs.h"
#include "memory.h"
#include "vga.h"

#define MAX_FILES 16

struct File {
    char name[32];
    unsigned int size;
    char *data;
};

static struct File files[MAX_FILES];
static int file_count = 0;

static unsigned int str_len(const char *s) {
    unsigned int len = 0;
    while (s[len]) len++;
    return len;
}

static int str_cmp(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return *a == *b;
}

void fs_init() {
    file_count = 0;
}

int fs_create(const char *name, const char *data) {
    if (file_count >= MAX_FILES) return -1;
    struct File *f = &files[file_count++];
    unsigned int i = 0;
    for (; i < 31 && name[i]; i++) {
        f->name[i] = name[i];
    }
    f->name[i] = '\0';
    f->size = str_len(data);
    f->data = (char*)malloc_simple(f->size + 1);
    for (i = 0; i < f->size + 1; i++) {
        f->data[i] = data[i];
    }
    return 0;
}

const char* fs_read(const char *name) {
    for (int i = 0; i < file_count; i++) {
        if (str_cmp(files[i].name, name)) {
            return files[i].data;
        }
    }
    return 0;
}

int fs_get(const char *name, const char **data, unsigned int *size) {
    for (int i = 0; i < file_count; i++) {
        if (str_cmp(files[i].name, name)) {
            *data = files[i].data;
            *size = files[i].size;
            return 0;
        }
    }
    return -1;
}

void fs_list() {
    for (int i = 0; i < file_count; i++) {
        print_string(files[i].name);
        print_string("\n");
    }
}


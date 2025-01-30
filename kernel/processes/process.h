// process.h
#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

struct process {
    int id;
    void (*entry_point)();
    uint32_t *page_dir;
};

void create_process(void (*entry_point)());
void kill_process(int pid);

#endif
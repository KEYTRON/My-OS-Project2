#ifndef TASK_H
#define TASK_H

typedef void (*task_func)();

void tasking_init();
void task_create(task_func func);
void task_run_once();

#endif

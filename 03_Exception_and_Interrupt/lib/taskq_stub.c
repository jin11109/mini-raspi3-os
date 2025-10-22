#include "../kernel/taskq.h"
__attribute__((weak)) void enqueue_task(task_t* t) {};
__attribute__((weak)) void process_task() {};
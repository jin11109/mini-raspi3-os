#ifndef _TASKQ_H
#define _TASKQ_H

#include "def.h"

#define MAX_TASKS 256

typedef void (*task_cb_t)(void* arg0, void* arg1);
typedef void (*task_unmask_cb_t)(void);

/* Priority of task */
typedef enum {
    TPRIO_LOW = 0,
    TPRIO_NORMAL = 1,
    TPRIO_HIGH = 2,
    TPRIO_COUNT = 3
} task_prio_t;

typedef struct {
        void* cb;
        void* arg0;
        void* arg1;
        task_prio_t prio;
        void* unmask_cb;
} task_t;

typedef struct {
    task_t buffer[MAX_TASKS];
    volatile int head;
    volatile int tail;
} ring_taskq_t;

void init_taskq();
void enqueue_task(task_t t);
void process_task();

#endif /* _TASKQ_H */

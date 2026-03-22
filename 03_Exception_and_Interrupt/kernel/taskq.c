#include "../kernel/taskq.h"

#include "def.h"
#include "irq.h"
#include "malloc.h"
#ifdef TEST_INTERRUPT
#include "mini_uart.h"
#include "utils.h"
#endif

/* Dummy node */
task_node_t* taskq[TPRIO_COUNT];

#ifdef TEST_INTERRUPT
/* Number of nesting  */
static int irq_nesting = 0;
static int last_prio = TPRIO_COUNT;
#endif

static int is_processing = 0;

void init_taskq() {
    /* Init dummy node */
    for (int i = 0; i < TPRIO_COUNT; i++) {
        taskq[i] = (task_node_t*)malloc(sizeof(task_node_t));
        *taskq[i] = (task_node_t){.t = NULL, .next = NULL};
    }
}

/* Fifo */
void enqueue_task(task_t* t) {
    task_node_t* n = taskq[t->prio];
    /* Traverse to end of queue */
    for (; n->next != NULL; n = n->next);
    /* TODO: dummy node doesn't malloc */
    n->next = (task_node_t*)malloc(sizeof(task_node_t));

    *(n->next) = (task_node_t){.t = t, .next = NULL};
}

static void dequeue_task(task_node_t* head) {
    task_node_t* n = head->next;
    head->next = n->next;
    free(n->t);
    free(n);
}

void process_task() {
    disable_irq;
#ifdef TEST_INTERRUPT
    irq_nesting++;
#endif
    if (is_processing) {
        enable_irq;
        return; 
    }
    is_processing = 1;
    enable_irq;

    for (int i = TPRIO_COUNT - 1; i >= 0; i--) {
        while (taskq[i]->next) {
            disable_irq;
            task_t t = *(taskq[i]->next->t);
            /**
             * We need to dequeue the task here instead of at the end of loop.
             * If a new interrupt occurs while the task is being processed, the
             * system call process_task() again, which could cause the same task
             * to run twice.
             */
            dequeue_task(taskq[i]);
#ifdef TEST_INTERRUPT
            if (irq_nesting > 1 && last_prio != TPRIO_COUNT &&
                t.prio > last_prio)
                printf_sync("[test interrupt] Task preempt\r\n");
            last_prio = t.prio;
#endif
            enable_irq;

            if (t.cb) ((task_cb_t)t.cb)(t.arg0, t.arg1);
            // Enalbe this interrupt
            if (t.unmask_cb) ((task_unmask_cb_t)(t.unmask_cb))();
        }
    }

    disable_irq;
    is_processing = 0;
#ifdef TEST_INTERRUPT
    /* critical seciton */
    irq_nesting--;
    if (irq_nesting != 0) {
        printf_sync("[test interrupt] Nest interrupt %d\r\n", irq_nesting);
    }
#endif
    enable_irq;
}
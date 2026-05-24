#include "kernel/taskq.h"

#include "kernel/irqflags.h"

#include "def.h"

#ifdef TEST_INTERRUPT
#include "drivers/mini_uart.h"
#include "utils.h"
#endif

#ifdef TEST_INTERRUPT
/* Number of nesting  */
static int irq_nesting = 0;
static int last_prio = TPRIO_COUNT;
#endif

static ring_taskq_t taskq[TPRIO_COUNT];
static int is_processing = 0;

void init_taskq() {
    for (int i = 0; i < TPRIO_COUNT; i++) {
        taskq[i].head = 0;
        taskq[i].tail = 0;
    }
}

/* Fifo */
void enqueue_task(task_t t) {
    disable_irq();

    int next_tail = (taskq[t.prio].tail + 1) % MAX_TASKS;

    // If queue is full, drop task
    if (next_tail == taskq[t.prio].head) {
        enable_irq();
        /* TODO: need to handle error*/
        return;
    }

    taskq[t.prio].buffer[taskq[t.prio].tail] = t;
    taskq[t.prio].tail = next_tail;

    enable_irq();
}

void process_task() {
    disable_irq();
#ifdef TEST_INTERRUPT
    irq_nesting++;
#endif
    if (is_processing) {
        enable_irq();
        return;
    }
    is_processing = 1;
    enable_irq();

    int has_pending_task;
    do {
        has_pending_task = 0;
        for (int i = TPRIO_COUNT - 1; i >= 0; i--) {
            while (1) {
                disable_irq();
                if (taskq[i].head == taskq[i].tail) {
                    enable_irq();
                    break;
                }

                /**
                 * We need to dequeue the task here instead of at the end of
                 * loop. If a new interrupt occurs while the task is being
                 * processed, the system call process_task() again, which could
                 * cause the same task to run twice.
                 */
                task_t t = taskq[i].buffer[taskq[i].head];
                taskq[i].head = (taskq[i].head + 1) % MAX_TASKS;
                has_pending_task = 1;

#ifdef TEST_INTERRUPT
                if (irq_nesting > 1 && last_prio != TPRIO_COUNT &&
                    t.prio > last_prio)
                    printf_sync("[test interrupt] Task preempt\r\n");
                last_prio = t.prio;
#endif
                enable_irq();

                if (t.cb) ((task_cb_t)t.cb)(t.arg0, t.arg1);
                // Enalbe this interrupt
                if (t.unmask_cb) ((task_unmask_cb_t)(t.unmask_cb))();
            }
        }
    } while (has_pending_task);

    disable_irq();
    is_processing = 0;
#ifdef TEST_INTERRUPT
    /* critical seciton */
    irq_nesting--;
    if (irq_nesting != 0) {
        printf_sync("[test interrupt] Nest interrupt %d\r\n", irq_nesting);
    }
#endif
    enable_irq();
}

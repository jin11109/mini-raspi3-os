#include "timer.h"

#include "../kernel/irq.h"
#include "../kernel/taskq.h"
#include "command_registry.h"
#include "def.h"
#include "malloc.h"
#include "mm.h"
#include "peripherals/timer.h"
#include "string.h"
#include "utils.h"

#define CORE0_TIMER_IRQ_CTRL 0x40000040

static timer_event_t* timer_queue = NULL;

static inline void set_hardware_timer(uint32_t ticks) {
    // Write expire time
    asm volatile("msr CNTP_TVAL_EL0, %0" ::"r"(ticks));
    // Enable timer
    asm volatile("msr CNTP_CTL_EL0, %0" ::"r"(1));
}

static inline uint64_t get_current_time() {
    uint64_t cnt;
    asm volatile("mrs %0, CNTPCT_EL0" : "=r"(cnt));
    return cnt;
}

static inline uint64_t get_freq() {
    uint64_t freq;
    asm volatile("mrs %0, CNTFRQ_EL0" : "=r"(freq));
    return freq;
}

static inline uint64_t get_current_time_in_us() {
    uint64_t cnt, freq;
    asm volatile("mrs %0, CNTPCT_EL0" : "=r"(cnt));
    asm volatile("mrs %0, CNTFRQ_EL0" : "=r"(freq));
    return (cnt * 1000000) / freq;
}

static inline void disable_hardware_timer() {
    asm volatile("msr CNTP_CTL_EL0, %0" ::"r"(0));
}

void add_timer(timer_callback_t cb, void* data, uint64_t after_ticks) {
    uint64_t now = get_current_time();
    uint64_t expire = now + after_ticks;

    timer_event_t* new_event = (timer_event_t*)malloc(sizeof(timer_event_t));

    new_event->expire_time = expire;
    new_event->callback = cb;
    new_event->data = data;
    new_event->next = NULL;

    /* TODO: Use minheap instead of traversing all of list*/
    if (!timer_queue || expire < timer_queue->expire_time) {
        new_event->next = timer_queue;
        timer_queue = new_event;
        set_hardware_timer(expire - get_current_time());
    } else {
        timer_event_t* cur = timer_queue;
        while (cur->next && cur->next->expire_time < expire) {
            cur = cur->next;
        }
        new_event->next = cur->next;
        cur->next = new_event;
    }
}

static void timer_bottom_handler(uintptr_t cb, uintptr_t arg) {
    ((timer_callback_t)cb)((void*)arg);
}

static void timer_unmask() {}

static void timer_interrupt_handler(void* arg) {
    uint64_t now = get_current_time();

    while (timer_queue && timer_queue->expire_time <= now) {
        timer_event_t* ev = timer_queue;
        timer_queue = ev->next;
        // Call callback function
        // ev->callback(ev->data);
        /* TODO: do not use malloc in top half */
        task_t* t = (task_t*)malloc(sizeof(task_t));
        *t = (task_t){.cb = (void*)timer_bottom_handler,
                      .arg0 = (void*)ev->callback,
                      .arg1 = (void*)ev->data,
                      .prio = TPRIO_HIGH,
                      .unmask_cb = (void*)timer_unmask};
        enqueue_task(t);

        free(ev->data);
        free(ev);
    }

    if (timer_queue) {
        set_hardware_timer(timer_queue->expire_time - get_current_time());
    } else {
        disable_hardware_timer();
    }
}

void init_timer() {
    // Enable core timer IRQ send to GIC/CPU
    MMIO_WRITE32(0x2u, CORE0_TIMER_IRQ_CTRL);

    local_irq_register_handler(CNTPNSIRQ, 0, timer_interrupt_handler, NULL);
}

void print_msg(void* msg) {
    printf_sync("[timeout] (prio:high) %s\r\n", (char*)msg);
}

void cmd_settimeout(int argc, char** argv) {
    if (argc == 2) {
        if (strcmp(argv[1], "--h") == 0) {
            printf("Usage:\r\nsetTimeout [MESSAGE] [SECONDS]\r\n");
        }
    } else if (argc == 3) {
        // Save message
        uint64_t len = strlen(argv[1]) + 1;
        char* msg = (char*)malloc(len);
        memcpy(msg, argv[1], len);

        uint64_t ticks = atou_dec64(argv[2], strlen(argv[2])) * get_freq();
        add_timer(print_msg, (void*)msg, ticks);
    }
}
COMMAND_DEFINE("settimeout", cmd_settimeout);
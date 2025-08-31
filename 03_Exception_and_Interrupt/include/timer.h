#ifndef _TIMER_H
#define _TIMER_H

#include "def.h"

typedef void (*timer_callback_t)(void *data);

typedef struct timer_event{
        uint64_t expire_time;
        timer_callback_t callback;
        void *data;
        struct timer_event *next;
} timer_event_t;

extern inline void init_timer();
void add_timer(timer_callback_t cb, void *data, uint64_t after_ticks);

#endif /* _TIMER_H */
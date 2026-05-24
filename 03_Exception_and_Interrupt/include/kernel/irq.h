#ifndef _INCLUDE_KERNEL_IRQ_H
#define _INCLUDE_KERNEL_IRQ_H

#include "def.h"

/* Used to register */
typedef void (*irq_handler_t)(void* arg);

void irq_handler();

void irq_register_handler(uint32_t irq, irq_handler_t handler, void* arg);
void local_irq_register_handler(uint32_t irq, uint32_t core, irq_handler_t fn,
                                void* arg);
void irq_unregister_handler(uint32_t irq);
void local_irq_unregister_handler(uint32_t irq, uint32_t core);

#endif /* _INCLUDE_KERNEL_IRQ_H */

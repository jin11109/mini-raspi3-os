#ifndef _IRQ_H
#define _IRQ_H

#include "def.h"

/* Used to register */
typedef void (*irq_handler_t)(void* arg);

/* TODO: Don't use these definition of controling all type of interrupts, such
 * as FIQ, IRQ...*/
#define disable_irq asm volatile("msr DAIFSet, #0xf")
#define enable_irq asm volatile("msr DAIFClr, #0xf")

void irq_handler();

void irq_register_handler(uint32_t irq, irq_handler_t handler, void* arg);
void local_irq_register_handler(uint32_t irq, uint32_t core, irq_handler_t fn,
                                void* arg);
void irq_unregister_handler(uint32_t irq);
void local_irq_unregister_handler(uint32_t irq, uint32_t core);

#endif /* _IRQ_H */

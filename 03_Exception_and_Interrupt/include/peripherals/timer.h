#ifndef _P_TIMER_H
#define _P_TIMER_H

#include "peripherals/local_intc.h"

#define CORE0_TIMER_IRQCNTL (0x40u + LOCAL_INTERRUPT_BASE)

/* Secure EL1 physical timer */
#define CNTPSIRQ 0
/* Non-secure EL1 physical timer */
#define CNTPNSIRQ 1
/* Hypervisor physical timer (EL2) */
#define CNTHPIRQ 2
/* Virtual timer (EL1 guest) */
#define CNTVIRQ 3

#endif /*_P_TIMER_H */
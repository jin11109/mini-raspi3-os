/* SoC interrupt controller (bcm2836-armctrl-ic) */
#ifndef _P_INTC_H
#define _P_INTC_H

#include "peripherals/base.h"
/* Define in BCM2837-ARM-Peripherals.-.Revised.-.V2-1 7.5 Registers page 112 */
#define INTERRUPT_BASE 0xB000u
#define IRQ_BASIC_PENDING (PBASE + INTERRUPT_BASE + 0x200u)
#define IRQ_PENDING1 (PBASE + INTERRUPT_BASE + 0x204u)
#define IRQ_PENDING2 (PBASE + INTERRUPT_BASE + 0x208u)
#define FIQ_CONTROL (PBASE + INTERRUPT_BASE + 0x20Cu)
#define ENABLE_IRQS_1 (PBASE + INTERRUPT_BASE + 0x210u)
#define ENABLE_IRQS_2 (PBASE + INTERRUPT_BASE + 0x214u)
#define ENABLE_BASIC_IRQS (PBASE + INTERRUPT_BASE + 0x218u)
#define DISABLE_IRQS_1 (PBASE + INTERRUPT_BASE + 0x21Cu)
#define DISABLE_IRQS_2 (PBASE + INTERRUPT_BASE + 0x220u)
#define DISABLE_BASIC_IRQS (PBASE + INTERRUPT_BASE + 0x224u)

#endif /*_P_INTC_H */

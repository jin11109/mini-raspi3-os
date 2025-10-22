/* ARM local interrupt controller (per-core)
 * https://datasheets.raspberrypi.com/bcm2836/bcm2836-peripherals.pdf
 */

#ifndef _P_LOCAL_INTC_H
#define _P_LOCAL_INTC_H

#define LOCAL_INTERRUPT_BASE 0x40000000u
#define CORE0_MAILBOX_IRQCNTL (0x50u + LOCAL_INTERRUPT_BASE)
#define CORE0_IRQ_SOURCE (0x60u + LOCAL_INTERRUPT_BASE)
#define CORE0_FIQ_SOURCE (0x70u + LOCAL_INTERRUPT_BASE)

#endif /*_P_LOCAL_INTC_H */

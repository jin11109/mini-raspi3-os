#ifndef _INCLUDE_KERNEL_IRQFLAGS_H
#define _INCLUDE_KERNEL_IRQFLAGS_H

/* TODO: Don't use these definition of controling all type of interrupts, such
 * as FIQ, IRQ...*/
#define disable_irq()               \
    do {                            \
        asm volatile(               \
            "msr DAIFSet, #0xf\n\t" \
            "dsb sy\n\t"            \
            "isb"                   \
            :                       \
            :                       \
            : "memory");            \
    } while (0)

#define enable_irq()                \
    do {                            \
        asm volatile(               \
            "msr DAIFClr, #0xf\n\t" \
            "dsb sy\n\t"            \
            "isb"                   \
            :                       \
            :                       \
            : "memory");            \
    } while (0)

#define disable_all_exceptions()    \
    do {                            \
        asm volatile(               \
            "msr DAIFSet, #0xf\n\t" \
            "dsb sy\n\t"            \
            "isb"                   \
            :                       \
            :                       \
            : "memory");            \
    } while (0)

#define enable_all_exceptions()     \
    do {                            \
        asm volatile(               \
            "msr DAIFClr, #0xf\n\t" \
            "dsb sy\n\t"            \
            "isb"                   \
            :                       \
            :                       \
            : "memory");            \
    } while (0)

#endif /* _INCLUDE_KERNEL_IRQFLAGS_H */

#include "irq.h"

#include "def.h"
#include "peripherals/intc.h"
#include "peripherals/local_intc.h"
#include "taskq.h"
#include "utils.h"

#define MAX_IRQS 64
#define MAX_LOCAL_IRQ 13
#define CPU_CORES 4

typedef void (*irq_handler_t)(void* arg);

typedef struct {
        irq_handler_t handler;
        void* arg;
} irq_entry_t;

static irq_entry_t irq_table[MAX_IRQS];
static irq_entry_t local_irq_table[CPU_CORES][MAX_LOCAL_IRQ];

void irq_register_handler(uint32_t irq, irq_handler_t fn, void* arg) {
    irq_table[irq] = (irq_entry_t){.handler = fn, .arg = arg};
    if (irq < 32)
        MMIO_WRITE32_OR(1 << irq, ENABLE_IRQS_1);
    else
        MMIO_WRITE32_OR(1 << (irq - 32), ENABLE_IRQS_2);
}

void local_irq_register_handler(uint32_t irq, uint32_t core, irq_handler_t fn,
                                void* arg) {
    local_irq_table[core][irq] = (irq_entry_t){.handler = fn, .arg = arg};
}

void irq_unregister_handler(uint32_t irq) {
    irq_table[irq] = (irq_entry_t){.handler = 0, .arg = 0};
    if (irq < 32)
        MMIO_WRITE32_OR(1 << irq, DISABLE_IRQS_1);
    else
        MMIO_WRITE32_OR(1 << (irq - 32), DISABLE_IRQS_2);
}

void local_irq_unregister_handler(uint32_t irq, uint32_t core) {
    local_irq_table[core][irq] = (irq_entry_t){.handler = 0, .arg = 0};
}

/**
 * C-level IRQ entry called from exception.S (with registers saved and
 * interrupts masked)
 * Scan for finfing what interrupt occurs
 */
void irq_handler() {
    /* Local IRQ */
    /* TODO: add muti-core */
    uint32_t core0_irq_src = MMIO_READ32(CORE0_IRQ_SOURCE);
    /* SoC IRQ */
    uint32_t basic = MMIO_READ32(IRQ_BASIC_PENDING);

    if (core0_irq_src) {
        /* TODO: consider using CTX */
        for (int i = 0; i < MAX_LOCAL_IRQ; i++) {
            if (core0_irq_src & (1 << i)) {
                if (local_irq_table[0][i].handler)
                    local_irq_table[0][i].handler(local_irq_table[0][i].arg);
            }
        }
    }

    if (basic & (1 << 8)) { // pending1
        uint32_t pending1 = MMIO_READ32(IRQ_PENDING1);
        for (int i = 0; i < 32; i++) {
            if (pending1 & (1 << i)) {
                if (irq_table[i].handler)
                    irq_table[i].handler(irq_table[i].arg);
            }
        }
    }

    if (basic & (1 << 9)) { // pending2
        uint32_t pending2 = MMIO_READ32(IRQ_PENDING2);
        for (int i = 0; i < 32; i++) {
            if (pending2 & (1 << i)) {
                int irq = i + 32;
                if (irq_table[irq].handler)
                    irq_table[irq].handler(irq_table[irq].arg);
            }
        }
    }
}

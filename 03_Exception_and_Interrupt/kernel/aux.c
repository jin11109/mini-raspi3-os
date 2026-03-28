#include "aux.h"

#include "def.h"
#include "irq.h"
#include "mini_uart.h"
#include "peripherals/aux.h"
#include "peripherals/mini_uart.h"
#include "taskq.h"
#include "utils.h"

void aux_init(void) {
    irq_register_handler(AUX_IRQ_ID, (irq_handler_t)aux_irq_handler, NULL);
}

void aux_irq_handler(void* arg) {
    uint32_t iir = MMIO_READ32(AUX_MU_IIR_REG);
    int budget = 64;
    while (!(iir & 0x1) && budget > 0) {
        switch ((iir >> 1) & 0x3) {
            case 1: // TX ready
                mini_uart_tx_top();
                break;
            case 2: // RX ready
                mini_uart_rx_top();
                break;
            case 3: // Line status
                MMIO_READ32(AUX_MU_LSR_REG);
                break;
        }
        budget--;
        iir = MMIO_READ32(AUX_MU_IIR_REG);
    }
}

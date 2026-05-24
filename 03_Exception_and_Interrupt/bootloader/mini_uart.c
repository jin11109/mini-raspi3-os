#include "mini_uart.h"

#include "peripherals/gpio.h"
#include "peripherals/mini_uart.h"

#include "utils.h"

// ----------------------------
// Flush mini uart api
// ----------------------------

void mini_uart_flush_recv(void) {
    for (int i = 0; i < 16; i++) {
        if (MMIO_READ32(AUX_MU_LSR_REG) & 0x01u) {
            MMIO_READ32(AUX_MU_IO_REG);
        }
    }
}
void mini_uart_flush_send(void) {
    while (!(MMIO_READ32(AUX_MU_LSR_REG) & (1u << 6)));
}

/* Initialize mini uart */
void mini_uart_init(void) {
    unsigned int selector;

    selector = MMIO_READ32(GPFSEL1);
    selector &= ~(7 << 12);  // clean gpio14
    selector |= 2 << 12;     // set alt5 for gpio14
    selector &= ~(7 << 15);  // clean gpio15
    selector |= 2 << 15;     // set alt5 for gpio15
    MMIO_WRITE32(selector, GPFSEL1);

    // Remove both the pull-up and pull-down states from pins14 and pins15
    MMIO_WRITE32(0, GPPUD);
    delay(150);
    MMIO_WRITE32((1 << 14) | (1 << 15), GPPUDCLK0);
    delay(150);
    MMIO_WRITE32(0, GPPUDCLK0);

    MMIO_WRITE32(1, AUX_ENABLES);  // Enable mini uart (this also enables access
                                   // to its registers)
    MMIO_WRITE32(0, AUX_MU_CNTL_REG);  // Disable auto flow control and disable
                                       // receiver and transmitter (for now)
    MMIO_WRITE32(0, AUX_MU_IER_REG);  // Disable receive and transmit interrupts
    MMIO_WRITE32(3, AUX_MU_LCR_REG);  // Enable 8 bit mode
    MMIO_WRITE32(0, AUX_MU_MCR_REG);  // Set RTS line to be always high
    MMIO_WRITE32(270, AUX_MU_BAUD_REG);  // Set baud rate to 115200

    MMIO_WRITE32(3,
                 AUX_MU_CNTL_REG);  // Finally, enable transmitter and receiver

    // Flush RX FIFO after enabling receiver
    mini_uart_flush_send();
    mini_uart_flush_recv();
}

// ----------------------------
// Sync mini uart api
// ----------------------------

/* Blocking write a character */
void mini_uart_sync_write(const char c) {
    while (1) {
        if (MMIO_READ32(AUX_MU_LSR_REG) & 0x20u) break;
    }
    MMIO_WRITE32(c, AUX_MU_IO_REG);
}

/* Blocking write a string */
void mini_uart_sync_write_str(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        mini_uart_sync_write((const char)str[i]);
    }
}

/* Blocking read a character */
char mini_uart_sync_read(void) {
    while (1) {
        if (MMIO_READ32(AUX_MU_LSR_REG) & 0x01u) break;
    }
    return (MMIO_READ32(AUX_MU_IO_REG) & 0xFFu);
}

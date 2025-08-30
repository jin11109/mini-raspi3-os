#include "peripherals/mini_uart.h"

#include "def.h"
#include "mini_uart.h"
#include "peripherals/gpio.h"
#include "utils.h"

CircularBuffer read_buffer = {0};
CircularBuffer write_buffer = {0};

/* Initialize mini uart */
void mini_uart_init(void) {
    unsigned int selector;

    selector = get32(GPFSEL1);
    selector &= ~(7 << 12); // clean gpio14
    selector |= 2 << 12;    // set alt5 for gpio14
    selector &= ~(7 << 15); // clean gpio15
    selector |= 2 << 15;    // set alt5 for gpio15
    put32(GPFSEL1, selector);

    // Remove both the pull-up and pull-down states from pins14 and pins15
    put32(GPPUD, 0);
    delay(150);
    put32(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);
    put32(GPPUDCLK0, 0);

    put32(AUX_ENABLES,
          1); // Enable mini uart (this also enables access to its registers)
    put32(AUX_MU_CNTL_REG, 0); // Disable auto flow control and disable receiver
                               // and transmitter (for now)
    put32(AUX_MU_IER_REG, 0);  // Disable receive and transmit interrupts
    put32(AUX_MU_LCR_REG, 3);  // Enable 8 bit mode
    put32(AUX_MU_MCR_REG, 0);  // Set RTS line to be always high
    put32(AUX_MU_BAUD_REG, 270); // Set baud rate to 115200

    put32(AUX_MU_CNTL_REG, 3); // Finally, enable transmitter and receiver

    // Flush RX FIFO after enabling receiver
    mini_uart_flush_send();
    mini_uart_flush_recv();
}

/* Blocking write a character */
void mini_uart_sync_write(const char c) {
    while (1) {
        if (get32(AUX_MU_LSR_REG) & 0x20) break;
    }
    put32(AUX_MU_IO_REG, c);
}

/* Blocking write a string */
void mini_uart_sync_write_str(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        mini_uart_sync_write((const char)str[i]);
    }
}

/* Blocking read a character */
char mini_uart_sync_read(void) {
    while (1) {
        if (get32(AUX_MU_LSR_REG) & 0x01) break;
    }
    return (get32(AUX_MU_IO_REG) & 0xFF);
}

/* Initialize async mini uart */
void mini_uart_async_init(void) {
    // Enble rx and tx interrupt
    *(volatile unsigned int *)AUX_MU_IER_REG = 0x3;
    // Enble mini uart interrupt
    *(volatile unsigned int *)IRQ_ENABLE1 |= (1 << 29);
}

void mini_uart_async_write(const char c) {
    if (write_buffer.count < MINI_UART_BUFFER_SIZE) {
        write_buffer.data[write_buffer.head] = c;
        write_buffer.head = (write_buffer.head + 1) % MINI_UART_BUFFER_SIZE;
        write_buffer.count++;
    }
    // Enable tx interrupt
    *(volatile unsigned int *)AUX_MU_IER_REG |= 0x2;
}

void mini_uart_async_write_str(const char *str) {
    while (*str && write_buffer.count < MINI_UART_BUFFER_SIZE) {
        write_buffer.data[write_buffer.head] = *str++;
        write_buffer.head = (write_buffer.head + 1) % MINI_UART_BUFFER_SIZE;
        write_buffer.count++;
    }
    // Enable tx interrupt
    *(volatile unsigned int *)AUX_MU_IER_REG |= 0x2;
}

/* Return number of bytes after read. */
int mini_uart_async_read(char *buf, int size) {
    int i;
    for (i = 0; i < size && read_buffer.count > 0; i++) {
        buf[i] = read_buffer.data[read_buffer.tail];
        read_buffer.tail = (read_buffer.tail + 1) % MINI_UART_BUFFER_SIZE;
        read_buffer.count--;
    }
    return i;
}

/**
 * When type on the keyboard, kernel will receive rx interrupt and will call
 * below function.
 */
void mini_uart_rx_handler(void) {
    while (*(volatile unsigned int *)AUX_MU_LSR_REG & 0x01) { // 有數據可讀
        unsigned char c = *(volatile unsigned int *)AUX_MU_IO_REG;
        if (read_buffer.count < MINI_UART_BUFFER_SIZE) {
            read_buffer.data[read_buffer.head] = c;
            read_buffer.head = (read_buffer.head + 1) % MINI_UART_BUFFER_SIZE;
            read_buffer.count++;
        }
    }
}

/**
 * When transmit FIFO is empty(AUX_MU_LSR_REG bit5 = 1) and TX interrupt enable,
 * it will cause tx interrupt.
 */
void mini_uart_tx_handler(void) {
    /**
     * When AUX_MU_LSR_REG bit5 (Transmitter empty) is 0, it means mini uart
     * is sending a character now. After the character has sent, bit5 turns back
     * to 1. Finally, send tx interrupt again.
     */
    if (write_buffer.count > 0 &&
        (*(volatile unsigned int *)AUX_MU_LSR_REG & 0x20)) {
        *(volatile unsigned int *)AUX_MU_IO_REG =
            write_buffer.data[write_buffer.tail];
        write_buffer.tail = (write_buffer.tail + 1) % MINI_UART_BUFFER_SIZE;
        write_buffer.count--;
    }
    if (write_buffer.count == 0) {
        // Unable TX interrupt
        *(volatile unsigned int *)AUX_MU_IER_REG &= ~0x2; // 禁用 TX 中斷
    }
}

void mini_uart_flush_recv(void) {
    for (int i = 0; i < 16; i++) {
        if (get32(AUX_MU_LSR_REG) & 0x01) {
            get32(AUX_MU_IO_REG);
        }
    }
}
void mini_uart_flush_send(void) { while (!(get32(AUX_MU_LSR_REG) & (1 << 6))); }
#include "peripherals/mini_uart.h"

/* TODO : Don't use kernel header file */
#include "../kernel/irq.h"
#include "../kernel/taskq.h"
#include "def.h"
#include "malloc.h"
#include "mini_uart.h"
#include "peripherals/gpio.h"
#include "peripherals/intc.h"
#include "utils.h"

// ----------------------------
// UART Device
// ----------------------------

typedef struct uart_device {
        CircularBuffer rx_buf;
        CircularBuffer tx_buf;
} uart_device_t;
static struct uart_device mini_uart_dev = {0};

// ----------------------------
// Interrupt mask
// ----------------------------

// disable rx interrupt
static inline void mini_uart_rx_mask() {
    MMIO_WRITE32_AND(~0x1u, AUX_MU_IER_REG);
}

// Enable rx interrupt
static void mini_uart_rx_unmask() { MMIO_WRITE32_OR(0x1u, AUX_MU_IER_REG); }

// disable tx interrupt
static inline void mini_uart_tx_mask() {
    MMIO_WRITE32_AND(~0x2u, AUX_MU_IER_REG);
}

// Enable tx interrupt
static void mini_uart_tx_unmask() { MMIO_WRITE32_OR(0x2, AUX_MU_IER_REG); }

// ----------------------------
// Init
// ----------------------------

/* Initialize mini uart */
void mini_uart_init(void) {
    unsigned int selector;

    selector = MMIO_READ32(GPFSEL1);
    selector &= ~(7 << 12); // clean gpio14
    selector |= 2 << 12;    // set alt5 for gpio14
    selector &= ~(7 << 15); // clean gpio15
    selector |= 2 << 15;    // set alt5 for gpio15
    MMIO_WRITE32(selector, GPFSEL1);

    // Remove both the pull-up and pull-down states from pins14 and pins15
    MMIO_WRITE32(0, GPPUD);
    delay(150);
    MMIO_WRITE32((1 << 14) | (1 << 15), GPPUDCLK0);
    delay(150);
    MMIO_WRITE32(0, GPPUDCLK0);

    MMIO_WRITE32(1, AUX_ENABLES); // Enable mini uart (this also enables access
                                  // to its registers)
    MMIO_WRITE32(0, AUX_MU_CNTL_REG); // Disable auto flow control and disable
                                      // receiver and transmitter (for now)
    MMIO_WRITE32(0, AUX_MU_IER_REG);  // Disable receive and transmit interrupts
    MMIO_WRITE32(3, AUX_MU_LCR_REG);  // Enable 8 bit mode
    MMIO_WRITE32(0, AUX_MU_MCR_REG);  // Set RTS line to be always high
    MMIO_WRITE32(270, AUX_MU_BAUD_REG); // Set baud rate to 115200

    MMIO_WRITE32(3,
                 AUX_MU_CNTL_REG); // Finally, enable transmitter and receiver

    // Flush RX FIFO after enabling receiver
    mini_uart_flush_send();
    mini_uart_flush_recv();

    /* TODO: Enble mini uart interrupt here (Remove mini_uart_async_init()) */
}

/* Initialize async mini uart */
void mini_uart_async_init(void) {
    /**
     * Enable rx here only, because tx interrupt will enable by calling mini
     * uart write api.
     */
    mini_uart_rx_unmask();

    /* XXX : Is this already be control in GIC? */
    // Enble mini uart interrupt
    MMIO_WRITE32_OR(1u << 29, ENABLE_IRQS_1);
}

// ----------------------------
// Bottom-half Workers
// ----------------------------

/* Unsed now. Because we only need to write data in the buffer. We don't parser
 * or process data. */
#ifdef TEST_INTERRUPT
static void mini_uart_rx_bottom(uintptr_t dev) {
    disable_irq;
    printf_sync("[test interrupt] (prio:normal) mini uart rx bottom start\r\n");
    enable_irq;
    /* Simulation of time consuming process */
    int sum = 0;
    for (int i = 1; i < 2000000000; i++) {
        sum += i;
    }
    disable_irq;
    /* Print sum to concern that for-loop has been excuted */
    printf_sync("[test interrupt] (prio:normal) mini uart rx bottom end %d\r\n",
                sum);
    enable_irq;
}
#endif

/**
 * This task only enble interrupt when it need to wait for mini uart. If it
 * successfully send all data in buffer, interrupt will be disable until mini
 * uart write api been call.
 */
static void mini_uart_tx_bottom(void* dev, void* unuse) {
    CircularBuffer* buffer = (CircularBuffer*)dev;
#ifdef TEST_INTERRUPT
    printf_sync("[test interrupt] (prio:low) mini uart tx bottom start\r\n");
    int sum = 0;
#endif
    while (buffer->count > 0) {
        if (MMIO_READ32(AUX_MU_LSR_REG) & 0x20u) {
            MMIO_WRITE32(buffer->data[buffer->tail], AUX_MU_IO_REG);
            buffer->tail = (buffer->tail + 1) % MINI_UART_BUFFER_SIZE;
            buffer->count--;
        } else {
#ifdef TEST_INTERRUPT
            printf_sync(
                "[test interrupt] (prio:low) mini uart tx bottom end\r\n");
#endif
            /**
             * Need to wait for mini uart, so enable inetrrupt again (do not
             * busy wait).
             */
            mini_uart_tx_unmask();
            return;
        }
#ifdef TEST_INTERRUPT
        /* Simulation of time consuming process */
        for (int i = 1; i < 2000000000; i++) {
            sum += i;
        }
#endif
    }
#ifdef TEST_INTERRUPT
    printf_sync("[test interrupt] (prio:low) mini uart tx bottom end %d\r\n",
                sum);
#endif
}

// ----------------------------
// Top-half Handlers
// ----------------------------

/**
 * Main concern:
 * In this top-half, it should read data quickly. Otherwise the data will be
 * overwrite.
 *
 * Interrupt occur:
 * When type on the keyboard, kernel will receive rx interrupt and will call
 * function below.
 */
void mini_uart_rx_top(void) {
    mini_uart_rx_mask();

    /**
     * This while loop is not busy-wait. Because if rx interrupt occur, it means
     * that there are already some data. But we don't know how many bytes are
     * there can be read. However, it is at most 16 bytes data there. So, the
     * while loop is bounded.
     */
    while (MMIO_READ32(AUX_MU_LSR_REG) & 0x01u) {
        unsigned char c = MMIO_READ32(AUX_MU_IO_REG);
        if (mini_uart_dev.rx_buf.count < MINI_UART_BUFFER_SIZE) {
            mini_uart_dev.rx_buf.data[mini_uart_dev.rx_buf.head] = c;
            mini_uart_dev.rx_buf.head =
                (mini_uart_dev.rx_buf.head + 1) % MINI_UART_BUFFER_SIZE;
            mini_uart_dev.rx_buf.count++;
        }
    }
#ifdef TEST_INTERRUPT
    /* Schedule bottom-half */
    /* TODO: do not use malloc in top half */
    /* This section is useless. Just for test now */
    task_t* t = (task_t*)malloc(sizeof(task_t));
    *t = (task_t){.cb = (void*)mini_uart_rx_bottom,
                  .arg0 = (void*)&mini_uart_dev,
                  .arg1 = (void*)NULL,
                  .prio = TPRIO_NORMAL,
                  .unmask_cb = (void*)mini_uart_rx_unmask};
    enqueue_task(t);
#endif
#ifndef TEST_INTERRUPT
    mini_uart_rx_unmask();
#endif
}

/**
 * When a tx interrupt occured, disable tx interrupt. Then, just enqueue a task.
 * The task will try to send all data in buffer. If the task need to wait for
 * mini uart, enable interrupt again (do not busy wait).
 *
 * This version reduce the fequency of sending interrupt, and move the time
 * cosuming job into bottom half.
 */
void mini_uart_tx_top() {
    mini_uart_tx_mask();
    /* TODO: do not use malloc in top half */
    task_t* t = (task_t*)malloc(sizeof(task_t));
    /* Interrupt will enable by mini uart write api */
    *t = (task_t){.cb = (void*)mini_uart_tx_bottom,
                  .arg0 = (void*)&(mini_uart_dev.tx_buf),
                  .arg1 = (void*)NULL,
                  .prio = TPRIO_LOW,
                  .unmask_cb = (void*)NULL};
    enqueue_task(t);
}

#ifdef NOUSE
/**
 * We don't need this function now.
 *
 * Main concern:
 * We only write a byte of data in the top-half. This will send interrupt at a
 * high frequency. But it is easy to implement, because don't need top/bottom
 * half.
 *
 * Interrupt occur:
 * When transmit FIFO is empty(AUX_MU_LSR_REG bit5 = 1) and TX interrupt enable,
 * it will cause tx interrupt.
 */
void mini_uart_tx_all_in_top(void) {
    /**
     * When AUX_MU_LSR_REG bit5 (Transmitter empty) is 0, it means mini uart
     * is sending a character now. After the character has sent, bit5 turns back
     * to 1. Finally, send tx interrupt again.
     */
    if (mini_uart_dev.tx_buf.count > 0 &&
        (MMIO_READ32(AUX_MU_LSR_REG) & 0x20u)) {
        MMIO_WRITE32(mini_uart_dev.tx_buf.data[mini_uart_dev.tx_buf.tail],
                     AUX_MU_IO_REG);
        mini_uart_dev.tx_buf.tail =
            (mini_uart_dev.tx_buf.tail + 1) % MINI_UART_BUFFER_SIZE;
        mini_uart_dev.tx_buf.count--;
    }
    /* Only when the buffer is empty, uneble tx interrupt */
    if (mini_uart_dev.tx_buf.count == 0) {
        MMIO_WRITE32_AND(~0x2u, AUX_MU_IER_REG);
    }
}
#endif

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

// ----------------------------
// Async mini uart api
// ----------------------------

void mini_uart_async_write(const char c) {
    if (mini_uart_dev.tx_buf.count < MINI_UART_BUFFER_SIZE) {
        mini_uart_dev.tx_buf.data[mini_uart_dev.tx_buf.head] = c;
        mini_uart_dev.tx_buf.head =
            (mini_uart_dev.tx_buf.head + 1) % MINI_UART_BUFFER_SIZE;
        mini_uart_dev.tx_buf.count++;
    }
    // Enable tx interrupt
    mini_uart_tx_unmask();
}

void mini_uart_async_write_str(const char* str) {
    while (*str && mini_uart_dev.tx_buf.count < MINI_UART_BUFFER_SIZE) {
        mini_uart_dev.tx_buf.data[mini_uart_dev.tx_buf.head] = *str++;
        mini_uart_dev.tx_buf.head =
            (mini_uart_dev.tx_buf.head + 1) % MINI_UART_BUFFER_SIZE;
        mini_uart_dev.tx_buf.count++;
    }
    // Enable tx interrupt
    mini_uart_tx_unmask();
}

/* Return number of bytes after read. */
int mini_uart_async_read(char* buf, int size) {
    int i;
    for (i = 0; i < size && mini_uart_dev.rx_buf.count > 0; i++) {
        buf[i] = mini_uart_dev.rx_buf.data[mini_uart_dev.rx_buf.tail];
        mini_uart_dev.rx_buf.tail =
            (mini_uart_dev.rx_buf.tail + 1) % MINI_UART_BUFFER_SIZE;
        mini_uart_dev.rx_buf.count--;
    }
    return i;
}

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
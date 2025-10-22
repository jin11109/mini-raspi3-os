#ifndef _MINI_UART_H
#define _MINI_UART_H

void mini_uart_init(void);

/* Blocking */
char mini_uart_sync_read(void);
void mini_uart_sync_write(const char c);
void mini_uart_sync_write_str(const char* str);

void mini_uart_flush_recv(void);
void mini_uart_flush_send(void);

#define MINI_UART_BUFFER_SIZE 256
typedef struct {
        unsigned char data[MINI_UART_BUFFER_SIZE];
        int head;
        int tail;
        int count;
} CircularBuffer;

extern CircularBuffer read_buffer;
extern CircularBuffer write_buffer;

/* IRQ top-half handler */
void mini_uart_rx_top(void);
void mini_uart_tx_top(void);

/* Non-blocking */
void mini_uart_async_init(void);
void mini_uart_async_write(const char str);
void mini_uart_async_write_str(const char* str);
int mini_uart_async_read(char* buf, int size);

#endif /*_MINI_UART_H */

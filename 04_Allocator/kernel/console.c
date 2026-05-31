#include "drivers/mini_uart.h"

void console_putchar_default(const char c) {
    mini_uart_async_write(c);
}

void console_putchar_sync(const char c) {
    mini_uart_sync_write(c);
}

void console_sendstr_default(const char* c) {
    mini_uart_async_write_str(c);
}

void console_sendstr_sync(const char* c) {
    mini_uart_sync_write_str(c);
}

char console_getchar_default(void) {
    char c_buf[1];
    while (1) {
        /* TODO: consider when cpu idel, run task queue:process_task();*/
        int len = mini_uart_async_read(c_buf, 1);
        if (len == 0) {
            continue;
        } else {
            break;
        }
    }
    return c_buf[0];
}

char console_getchar_sync(void) {
    return mini_uart_sync_read();
}

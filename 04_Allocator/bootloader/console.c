#include "mini_uart.h"

void console_putchar_default(const char c) {
    mini_uart_sync_write(c);
}

void console_putchar_sync(const char c) {
    mini_uart_sync_write(c);
}

void console_sendstr_default(const char* c) {
    mini_uart_sync_write_str(c);
}

void console_sendstr_sync(const char* c) {
    mini_uart_sync_write_str(c);
}

char console_getchar_default(void) {
    return mini_uart_sync_read();
}

char console_getchar_sync(void) {
    return mini_uart_sync_read();
}

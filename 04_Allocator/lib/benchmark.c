#include "command_registry.h"
#include "drivers/mini_uart.h"
#include "utils.h"

void benchmark_mini_uart_echo() {
    // printf_sync("Entering Echo Mode (Software Backpressure)...\r\n");
    while (1) {
        char c;
        int len = mini_uart_async_read(&c, 1);

        if (len > 0) {
            mini_uart_async_write(c);
        } else {
            __asm__ volatile("wfi");
        }
    }
}
COMMAND_DEFINE("test-mini-uart", benchmark_mini_uart_echo);

#include "mini_uart.h"
#include "shell.h"

__attribute__((section(".kernel.entry")))
void kernel_main(void) {
    uart_init();
    shell();
}

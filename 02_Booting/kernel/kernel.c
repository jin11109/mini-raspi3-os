#include "mini_uart.h"
#include "mm.h"
#include "shell.h"

extern char __kernel_bss_start, __kernel_bss_end;

__attribute__((section(".kernel.entry")))
void kernel_main(void) {
    __asm__(
        "ldr x0, =__kernel_stack_top \n"
        "mov sp, x0");
    memzero(__kernel_bss_start, __kernel_bss_end - __kernel_bss_start);
    shell();
}

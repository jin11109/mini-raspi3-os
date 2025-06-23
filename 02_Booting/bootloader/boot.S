#include "mm.h"

.section ".bootloader.entry"

.globl _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF // Check processor id
    cbz x0, master // Hang for all non-primary CPU
    b proc_hang

proc_hang:
    b proc_hang

master:
    adr x0, __bootloader_bss_start
    adr x1, __bootloader_bss_end
    sub x1, x1, x0
    bl memzero

    adr x0, __kernel_bss_start
    adr x1, __kernel_bss_end
    sub x1, x1, x0
    bl memzero

    mov sp, #LOW_MEMORY
    bl bootloader_main
    b proc_hang // should never come here

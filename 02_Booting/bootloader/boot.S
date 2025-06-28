.section ".bootloader.entry"
.extern bootloader_main
.extern memzero

.globl _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF // Check processor id
    cbz x0, master // Hang for all non-primary CPU
    b proc_hang

proc_hang:
    wfe
    b proc_hang

master:
    ldr x0, =__bootloader_stack_top
    mov sp, x0

    adr x0, __bootloader_bss_start
    adr x1, __bootloader_bss_end
    sub x1, x1, x0
    bl memzero

    bl bootloader_main
    b proc_hang // should never come here

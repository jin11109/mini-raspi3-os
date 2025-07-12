.section ".bootloader.entry"
.extern bootloader_main

.globl _bootloader_entry
_bootloader_entry:
    // Save arguments temporarily in callee-saved registers
    mov x20, x0 // preserve x0 (dtb addr)
    mov x21, x1 // reserved
    mov x22, x2 // reserved

    mrs x0, mpidr_el1
    and x0, x0, #0xFF // Check processor id
    cbz x0, master // Hang for all non-primary CPU
    b proc_hang

// Other cores
proc_hang:
    wfe
    b proc_hang

// Boot core
master:
    ldr x0, =__bootloader_stack_top
    mov sp, x0

    ldr x0, =__bootloader_bss_start
    ldr x1, =__bootloader_bss_end
    mov x2, #0
loop:
    str x2, [x0], #8
    cmp x0, x1
    b.lo loop

    mov x0, x20
    mov x1, x21
    mov x2, x22
    bl bootloader_main

    b proc_hang // should never come here

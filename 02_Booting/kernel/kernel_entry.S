.section ".kernel.entry"
.extern kernel_main
.extern memzero

.globl _kernel_entry
_kernel_entry:
    // Save arguments temporarily in callee-saved registers
    mov x20, x0 // preserve x0 (dtb addr)
    mov x21, x1 // reserved
    mov x22, x2 // reserved

    ldr x0, =__kernel_stack_top
    mov sp, x0

    ldr x0, =__kernel_bss_start
    ldr x1, =__kernel_bss_end
    mov x2, #0
loop:
    str x2, [x0], #8
    cmp x0, x1
    b.lo loop

    mov x0, x20
    mov x1, x21
    mov x2, x22
    bl kernel_main


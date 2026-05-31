.section ".bootloader.entry"

.extern bootloader_main

.global _bootloader_entry
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
    ldr x0, =__bootloader_start
    ldr x1, =__reserved_region_start

    // Set sp to relocation region
    ldr x2, =__bootloader_stack_top
    sub x2, x2, x0 // offset to bootloader start
    add x2, x2, x1
    mov sp, x2

    // Clear .bss
    ldr x2, =__bootloader_bss_start
    ldr x3, =__bootloader_bss_end
    mov x4, #0
zero_bss:
    str x4, [x2], #8
    cmp x2, x3
    b.lo zero_bss

    // Relocate self (bootloader)
    mov x2, x0 // src
    mov x3, x1 // dst
    ldr x4, =__bootloader_end
    sub x4, x4, x2 // size = end - start
copy_self:
    ldr x5, [x2], #8
    str x5, [x3], #8
    subs x4, x4, #8
    b.gt copy_self

    dsb sy
    isb

    // Branch to relocated bootloader_main (at new location)
    adr x2, bootloader_main
    sub x2, x2, x0 // offset to main
    add x7, x1, x2

    mov x0, x20
    mov x1, x21
    mov x2, x22

    br x7 // jump to relocated bootloader_main
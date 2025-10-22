// ----------------------------
// rodata
// ----------------------------
.section .rodata
.align 3
fmt_spsr:       .asciz "Test exception:\r\n    SPSR_EL1 = 0x%lx\r\n    ELR_EL1 = 0x%lx\r\n    ESR_EL1 = 0x%lx\r\n"
test:           .asciz "Test\r\n"

// ----------------------------
// text
// ----------------------------
.section .text
.macro save_all
    sub sp, sp, #272 // 256 (GPRs) + 16 (SPSR/ELR)
    stp x0, x1, [sp, 16*0]
    stp x2, x3, [sp, 16*1]
    stp x4, x5, [sp, 16*2]
    stp x6, x7, [sp, 16*3]
    stp x8, x9, [sp, 16*4]
    stp x10, x11, [sp, 16*5]
    stp x12, x13, [sp, 16*6]
    stp x14, x15, [sp, 16*7]
    stp x16, x17, [sp, 16*8]
    stp x18, x19, [sp, 16*9]
    stp x20, x21, [sp, 16*10]
    stp x22, x23, [sp, 16*11]
    stp x24, x25, [sp, 16*12]
    stp x26, x27, [sp, 16*13]
    stp x28, x29, [sp, 16*14]
    str x30, [sp, 16*15]
    mrs x0, spsr_el1
    mrs x1, elr_el1
    stp x0, x1, [sp, 16*16]
.endm

.macro load_all
    ldp x0, x1, [sp, 16*16]
    msr spsr_el1, x0
    msr elr_el1, x1
    ldp x0, x1, [sp, 16*0]
    ldp x2, x3, [sp, 16*1]
    ldp x4, x5, [sp, 16*2]
    ldp x6, x7, [sp, 16*3]
    ldp x8, x9, [sp, 16*4]
    ldp x10, x11, [sp, 16*5]
    ldp x12, x13, [sp, 16*6]
    ldp x14, x15, [sp, 16*7]
    ldp x16, x17, [sp, 16*8]
    ldp x18, x19, [sp, 16*9]
    ldp x20, x21, [sp, 16*10]
    ldp x22, x23, [sp, 16*11]
    ldp x24, x25, [sp, 16*12]
    ldp x26, x27, [sp, 16*13]
    ldp x28, x29, [sp, 16*14]
    ldr x30, [sp, 16*15]
    add sp, sp, #272
.endm

/* Debuger */
.global exception_handler
exception_handler:
    save_all
    bl exception_entry
    load_all
    eret

.global exception_entry
.extern printf_sync
exception_entry:
    // Mask all interrupt
    msr DAIFSet, #0xf

    // Load rodata
    adrp x0, fmt_spsr
    add x0, x0, :lo12:fmt_spsr

    // Call printf_sync
    /* XXX: This may be slow because mini uart io. */
    mrs x1, spsr_el1
    mrs x2, elr_el1
    mrs x3, esr_el1

    stp x30, xzr, [sp, #-16]! //caller save registers
    // bl printf_sync
    ldp x30, xzr, [sp], #16
    ret

/* Top half interrrupt handler */
.global irq_entry
irq_entry:
    // Disable cpu interrupts while processing top-half
    msr DAIFSet, #0xf

    save_all
    // Call top-half interrupt handler
    bl irq_handler
    
    // Enable cpu interrupts while processing bottom-half
    msr DAIFClr, #0xf
    // Run bottom-half works
    bl process_task
    
    load_all

    eret
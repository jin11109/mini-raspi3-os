// ----------------------------
// rodata
// ----------------------------
.section .rodata
.align 3
fmt_spsr: .asciz "[Exception Entry] Test exception:\r\n    SPSR_EL1 = 0x%lx\r\n    ELR_EL1 = 0x%lx\r\n    ESR_EL1 = 0x%lx\r\n"
svc_msg:  .asciz "[SVC] Unknown syscal: SVC #%d\r\n"

// ----------------------------
// text
// ----------------------------
.section .text

.include "context.S"

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

/* Print SVC only */
.global sync_handler
sync_handler:
    msr DAIFSet, #0xf
    // Save user(ELO) context
    save_all
    mrs x20, esr_el1

    // EC = bits[31:26]
    lsr x19, x20, #26 // x19 = EC
    // EC = 0b010101 (0x15): SVC instruction from AArch64 EL0
    cmp x19, #0x15
    b.ne sync_handler_end

    // Test SVC number
    ubfx x19, x20, #0, #16 // get SVC immediate
    cmp x19, #93
    b.eq sys_exit
    cmp x19, #64
    b.eq sys_write
    // Call printf_sync
    mov x1, x19
    adrp x0, svc_msg
    add x0, x0, :lo12:svc_msg
    /* XXX: This may be slow because mini uart io. */
    bl printf_sync

sync_handler_end:
    // Load user(EL0) context
    load_all
    msr DAIFClr, #0xf
    eret

/**
 * Syscall: write(fd, buf, len)
 * Temporarily handle printf_sync as a stand-in for the write syscall.  
 * Currently, only the 'buf' argument from the write syscall is used.  
 */ 
/* TODO: Implement a proper write syscall */
sys_write:
    mov x0, x1
    bl printf_sync

    // Load user(EL0) context
    load_all
    msr DAIFClr, #0xf
    eret

/* TODO: Test exit number */
sys_exit:
    msr DAIFClr, #0xf
    b return_to_shell
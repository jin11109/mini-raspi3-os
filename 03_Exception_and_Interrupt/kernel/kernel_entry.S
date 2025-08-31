.section ".kernel.entry"
.extern kernel_main

#define CORE0_TIMER_IRQ_CTRL 0x40000040

.globl _kernel_entry
_kernel_entry:
    // Save arguments temporarily in callee-saved registers
    mov x20, x0 // preserve x0 (dtb addr)
    mov x21, x1 // reserved
    mov x22, x2 // reserved

    // Set up el2 stack pointer
    ldr x0, =__el2_stack_top
    mov sp, x0

    // set_exception_vector_table
    adr x0, exception_vector_table
    msr vbar_el1, x0
    isb // ensure vbar_el1 enable immediately

    // Clear bss
    ldr x0, =__kernel_bss_start
    ldr x1, =__kernel_bss_end
    mov x2, #0
loop:
    str x2, [x0], #8
    cmp x0, x1
    b.lo loop

    bl from_el2_to_el1 // the next instruction runs in EL1

    bl core_timer_enable

    // Enble interrupt
    msr daifclr, #0xf

    // Call kernel_main function
    mov x0, x20
    mov x1, x21
    mov x2, x22
    bl kernel_main

from_el2_to_el1:
    mov x0, (1<<31)// EL1 uses aarch64
    msr hcr_el2, x0

    // Set up el1 stack pointer
    ldr x0, =__kernel_stack_top
    msr sp_el1, x0

    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0

    msr elr_el2, lr
    eret // return to EL1

core_timer_enable:
    mov x0, 1
    msr cntp_ctl_el0, x0 // enable
    mrs x0, cntfrq_el0
    lsl x0, x0, #1
    msr cntp_tval_el0, x0 // set expired time (2 seconds)
    
    mov x0, 2
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str w0, [x1]// unmask timer interrupt
    ret

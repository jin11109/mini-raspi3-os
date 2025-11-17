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
    mrs x19, spsr_el1
    mrs x20, elr_el1
    stp x19, x20, [sp, 16*16]
.endm

.macro load_all
    ldp x19, x20, [sp, 16*16]
    msr spsr_el1, x19
    msr elr_el1, x20
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

# If you want to use the value in these register after function call, you
# should save them first
# .macro CALLER_SAVE_ALL base
#     # x0 – x7   function arguments & return value
#     # x8        indirect result location
#     # x9 – x15  temporary registers (scratch)
#     # lr(x30)
#     stp x0,  x1,  [\base, #-16]!
#     stp x2,  x3,  [\base, #-16]!
#     stp x4,  x5,  [\base, #-16]!
#     stp x6,  x7,  [\base, #-16]!
#     stp x8,  x9,  [\base, #-16]!
#     stp x10, x11, [\base, #-16]!
#     stp x12, x13, [\base, #-16]!
#     stp x14, x15, [\base, #-16]!
#     stp x16, x17, [\base, #-16]!
#     stp x18, x30, [\base, #-16]!
# .endm
#
# .macro CALLER_LOAD_ALL base
#     ldp x18, x30, [\base], #16
#     ldp x16, x17, [\base], #16
#     ldp x14, x15, [\base], #16
#     ldp x12, x13, [\base], #16
#     ldp x10, x11, [\base], #16
#     ldp x8,  x9,  [\base], #16
#     ldp x6,  x7,  [\base], #16
#     ldp x4,  x5,  [\base], #16
#     ldp x2,  x3,  [\base], #16
#     ldp x0,  x1,  [\base], #16
# .endm

# If you want to use the value in these registe, you should save them and
# load them before return.
# .macro CALLEE_SAVE_ALL base
#     # X19 - X28  preserved registers
#     # fp (x29)   frame pointer
#     # lr (x30)    
#     # sp         stack pointer
#     stp x0,  x1,  [\base, #-16]!
#     stp x29, x30, [\base, #-16]!
#     stp x19, x20, [\base, #-16]!
#     stp x21, x22, [\base, #-16]!
#     stp x23, x24, [\base, #-16]!
#     stp x25, x26, [\base, #-16]!
#     stp x27, x28, [\base, #-16]!
#     mrs x0, spsr_el1
#     mrs x1, elr_el1
#     stp x0, x1, [\base, #-16]!
# .endm
#
# .macro CALLEE_LOAD_ALL base
#     ldp x0, x1, [\base], #16
#     msr spsr_el1, x0
#     msr elr_el1, x1
#     ldp x27, x28, [\base], #16
#     ldp x25, x26, [\base], #16
#     ldp x23, x24, [\base], #16
#     ldp x21, x22, [\base], #16
#     ldp x19, x20, [\base], #16
#     ldp x29, x30, [\base], #16
#     ldp x0,  x1,  [\base], #16
# .endm

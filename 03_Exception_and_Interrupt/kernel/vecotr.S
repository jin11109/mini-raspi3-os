.section .text

.align 11 // 2^11 = 0x800
.global exception_vector_table
exception_vector_table:
// EL1t
    b exception_handler // branch to a handler function.
.align 7 // entry size is 0x80, .align will pad 0
    b exception_handler
.align 7
    b exception_handler
.align 7
    b exception_handler
.align 7

// EL1h
    b exception_handler // Synchronous
.align 7
    b irq_entry // IRQ
.align 7
    b exception_handler // FIQ
.align 7
    b exception_handler // SError
.align 7

// EL0_64
    b exception_handler
.align 7
    b irq_entry
.align 7
    b exception_handler
.align 7
    b exception_handler
.align 7

// EL0_32
    b exception_handler
.align 7
    b exception_handler
.align 7
    b exception_handler
.align 7
    b exception_handler
.align 7

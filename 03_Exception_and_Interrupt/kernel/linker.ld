/**
 * This linker script links only kernel.
 */

ENTRY(_kernel_entry)

MEMORY {
    KERNEL (rwx) : ORIGIN = 0x80000, LENGTH = 1M
}

SECTIONS {
    .text : {
        __kernel_start = .;
        KEEP(*kernel/*.o(.kernel.entry))
        *kernel/*.o(.text*)
        *(.text*)
        . = ALIGN(8);
    } > KERNEL

    .rodata : {
        *kernel/*.o(.rodata*)
        *(.rodata*)        
        . = ALIGN(8);
        __command_start = .;
        KEEP(*(.command_table))
        __command_end = .;
    } > KERNEL

    .data : {
        *kernel/*.o(.data*)
        *(.data*)
        . = ALIGN(8);
    } > KERNEL

    .bss (NOLOAD) : {
        __kernel_bss_start = .;
        *kernel/*.o(.bss*)
        *(.bss*)
        . = ALIGN(8);
        __kernel_bss_end = .;
    } > KERNEL

    /* Heap */
    . = ALIGN(0x10);
    __kernel_heap_start = .;
    __kernel_heap_end = __kernel_heap_start + 96K;
    
    . = __kernel_heap_end;

    /* EL1 stack (32 KB) */
    __kernel_stack_top = ORIGIN(KERNEL) + LENGTH(KERNEL); /* 0x80000 + 1MB = 0x180000 */
    __kernel_stack_bottom = __kernel_stack_top - 32K;

    /* EL2 stack (4 KB) */
    __el2_stack_top = __kernel_stack_bottom;
    __el2_stack_bottom = __el2_stack_top - 4K;

    __kernel_end = .;
}

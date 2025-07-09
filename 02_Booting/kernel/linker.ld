/**
 * This linker script links only kernel into a image.
 */

ENTRY(_kernel_entry)

MEMORY {
    KERNEL (rwx) : ORIGIN = 0xa0000, LENGTH = 1M
}

SECTIONS {
	 /* kernel section */
    .kernel : {
        __kernel_start = .;
		KEEP(*kernel/*.o(.kernel.entry))
        *kernel/*.o(.text*)
        *(.text*)
        *kernel/*.o(.rodata*)
        *(.rodata*)
        *kernel/*.o(.data*)
		*(.data*)
        . = ALIGN(0x8);

        . = ALIGN(8);
        __command_start = .;
        KEEP(*(.command_table))
        __command_end = .;

		__kernel_bss_start = .;
        *kernel/*.o(.bss*)
        *(.bss*)	
		__kernel_bss_end = .;

        . = ALIGN(0x8);
        __kernel_heap_start = .;
        . = . + 48K;
        __kernel_heap_end = .;

        . = ALIGN(0x10);
        . = ORIGIN(KERNEL) + LENGTH(KERNEL) - 16K;
        __kernel_stack_bottom = .;
        . = . + 16K;
        __kernel_stack_top = .;
    } > KERNEL

    __kernel_end = .;
}

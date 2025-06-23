/**
 * This linker script links only kernel into a image.
 */

ENTRY(kernel_main)

SECTIONS {
	/* kernel section */
	. = 0x90000;
    __kernel_start = .;

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
		__kernel_bss_start = .;
        *kernel/*.o(.bss*)
        *(.bss*)	
		__kernel_bss_end = .;
    }

    __kernel_end = .;
}

/**
 * This linker script links the bootloader and kernel into a unified image.
 *
 * TODO: Refactor to generate separate image files for the bootloader and kernel.
 */

ENTRY(_start)

/** 
 * In order to limit the length of bootloader section. 
 * 0x80000 - 0x60000 = 128K
 */
MEMORY {
    BOOTLOADER (rwx) : ORIGIN = 0x80000, LENGTH = 64K
    KERNEL (rwx) : ORIGIN = 0x90000, LENGTH = 1M
}

SECTIONS {
    /* bootloader section */
    .bootloader : {
		KEEP(*bootloader/*.o(.bootloader.entry))
        *bootloader/*.o(.text*)
        *bootloader/*.o(.rodata*)
        *bootloader/*.o(.data*)
		. = ALIGN(0x8);
		__bootloader_bss_start = .;
        *bootloader/*.o(.bss*)
		__bootloader_bss_end = .;
    } > BOOTLOADER

    /* Kernel Section */

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
    } > KERNEL

    __kernel_end = .;
}

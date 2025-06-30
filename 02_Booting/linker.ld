/**
 * This linker script links the bootloader and kernel into a unified image.
 *
 * TODO: Refactor to generate separate image files for the bootloader and kernel.
 */

ENTRY(_start)

MEMORY {
    BOOTLOADER (rwx) : ORIGIN = 0x80000, LENGTH = 128K
    KERNEL (rwx) : ORIGIN = 0xa0000, LENGTH = 1M
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

        . = ALIGN(0x8);
        __bootloader_heap_start = .;
        . = . + 48K;
        __bootloader_heap_end = .;

        . = ORIGIN(BOOTLOADER) + LENGTH(BOOTLOADER) - 16K;
        __bootloader_stack_bottom = .;
        . = . + 16K;
        __bootloader_stack_top = .;
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

        . = ALIGN(8);
        __command_start = .;
        KEEP(*(.command_table))
        __command_end = .;

        . = ALIGN(0x8);
		__kernel_bss_start = .;
        *kernel/*.o(.bss*)
        *(.bss*)	
		__kernel_bss_end = .;

        . = ALIGN(0x8);
        __kernel_heap_start = .;
        . = . + 48K;
        __kernel_heap_end = .;

        . = ORIGIN(KERNEL) + LENGTH(KERNEL) - 16K;
        __kernel_stack_bottom = .;
        . = . + 16K;
        __kernel_stack_top = .;
    } > KERNEL

    __kernel_end = .;
}

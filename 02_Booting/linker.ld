/**
 * This linker script links the bootloader and kernel into a unified image.
 */

ENTRY(_bootloader_entry)

MEMORY {
    BOOTLOADER (rwx) : ORIGIN = 0x80000, LENGTH = 128K
    KERNEL_BLOB (rwx) : ORIGIN = 0xa0000, LENGTH = 1M
    RESERVED_REGION (rwx) : ORIGIN = 0x200000, LENGTH = 256K
}

SECTIONS {
    /* bootloader section */
    . = 0x80000;
    __kernel_start = .;
    __bootloader_start = .;
    .bootloader : {
		KEEP(*bootloader/*.o(.bootloader.entry))
        *bootloader/*.o(.text*)
        EXCLUDE_FILE(*kernel8_pure_blob.o) *(.text*)
        *bootloader/*.o(.rodata*)
        EXCLUDE_FILE(*kernel8_pure_blob.o) *(.rodata*)
        *bootloader/*.o(.data*)
        EXCLUDE_FILE(*kernel8_pure_blob.o) *(.data*)

        . = ALIGN(8);
        KEEP(*(.command_table))

        /* bss */
		. = ALIGN(0x8);
		__bootloader_bss_start = .;
        *bootloader/*.o(.bss*)
        EXCLUDE_FILE(*kernel8_pure_blob.o) *(.bss*)
		__bootloader_bss_end = .;

        /* heap */
        . = ALIGN(0x8);
        __bootloader_heap_start = .;
        . = . + 48K;
        __bootloader_heap_end = .;

        /* stack */
        . = ORIGIN(BOOTLOADER) + LENGTH(BOOTLOADER) - 16K;
        __bootloader_stack_bottom = .;
        . = . + 16K;
        __bootloader_stack_top = .;
    } > BOOTLOADER
    __bootloader_end = .;
    
    /* kernel blob section */
    . = 0xa0000;
    __kernel_blob_start = .;
    .kernel_blob : {
        KEEP(*kernel8_pure_blob.o(*))
    } > KERNEL_BLOB
    __kernel_blob_end = .;

    /* reserved region */
    . = 0x200000;
    __reserved_region_start = .;
    .reserved_region : {
        . = . + LENGTH(RESERVED_REGION);
    } > RESERVED_REGION
    __reserved_region_end = .;

}

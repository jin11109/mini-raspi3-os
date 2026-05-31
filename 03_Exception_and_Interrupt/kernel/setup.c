/**
 * Runs during the pre-malloc boot phase. It explicitly only parses:
 * - memory : To discover physical RAM for the early allocator.
 * - chosen : To locate the initramfs (ramdisk) and bootargs.
 *
 * Hardware peripherals (INTC, UART, etc.) are NOT parsed here. They are
 * deferred until the memory subsystem is initialized and the FDT is fully
 * unflattened.
 */

#include "setup.h"

#include "fdt.h"

#include "string.h"
#include "utils.h"

extern void early_init_dt_scan_chosen(const char *path, const char *prop,
                                      const void *data, uint32_t len);

static void early_dtb_scan_callback(const char *path, const char *prop_name,
                                    const void *data, uint32_t len,
                                    fdt_traverse_ctx_t *ctx) {
    if (strcmp(path, "chosen") == 0) {
        early_init_dt_scan_chosen(path, prop_name, data, len);
        return;
    }
}

void setup_arch(void *dtb_ptr) {
    fdt_traverse(dtb_ptr, early_dtb_scan_callback);
}

#include "cpio.h"
#include "fdt.h"
#include "string.h"
#include "utils.h"

void initramfs_callback(const char *path, const char *prop, const void *data,
                        uint32_t len) {
    if (strcmp(path, "/chosen/") != 0) return;

    if (strcmp(prop, "linux,initrd-start") == 0 && len == 4) {
        initramfs_start = __builtin_bswap32(*(const uint32_t *)data);
    } else if (strcmp(prop, "linux,initrd-end") == 0 && len == 4) {
        initramfs_end = __builtin_bswap32(*(const uint32_t *)data);
    }
}
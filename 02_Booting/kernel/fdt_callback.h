#ifndef _KERNEL_FDT_CALLBACK_H
#define _KERNEL_FDT_CALLBACK_H

#include "def.h"

void initramfs_callback(const char *path, const char *prop, const void *data,
                        uint32_t len);

#endif /* _KERNEL_FDT_CALLBACK_H */
#ifndef _KERNEL_FDT_H
#define _KERNEL_FDT_H

#include "def.h"

typedef void (*fdt_callback_t)(const char *path, const char *prop,
                               const void *data, uint32_t len);

#define FDT_MAGIC 0xd00dfeed
#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009

typedef struct {
        uint32_t magic;
        uint32_t totalsize;
        uint32_t off_dt_struct;
        uint32_t off_dt_strings;
        uint32_t off_mem_rsvmap;
        uint32_t version;
        uint32_t last_comp_version;
        uint32_t boot_cpuid_phys;
        uint32_t size_dt_strings;
        uint32_t size_dt_struct;
} __attribute__((packed)) fdt_header_t;

typedef struct {
        const char **path_stack; // dynamic array of current path segments
        int depth;               // current nesting depth
        int capacity;            // allocated capacity for path_stack
} fdt_traverse_ctx;

void fdt_traverse(void *fdt, fdt_callback_t callback);
char *fdt_build_path_alloc(fdt_traverse_ctx *ctx, const char *name);

#endif /* _KERNEL_FDT_H */

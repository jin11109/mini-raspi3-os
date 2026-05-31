#ifndef _KERNEL_FDT_H
#define _KERNEL_FDT_H

#include "def.h"

/** 
 * TODO: Refactor CPIO header using the "Macro Guard" pattern to encapsulate and
 * protect private internal variables and structures from external modules. 
 */
#define FDT_MAGIC 0xd00dfeed
#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009

#define MAX_FDT_DEPTH 16

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
    const char *name;
    uint32_t address_cells;
    uint32_t size_cells;
    const void *ranges_prop;
    uint32_t ranges_len;
} fdt_node_state_t;

typedef struct {
    fdt_node_state_t stack[MAX_FDT_DEPTH];
    int depth;
} fdt_traverse_ctx_t;

typedef void (*fdt_callback_t)(const char *path, const char *prop_name,
                               const void *data, uint32_t len,
                               fdt_traverse_ctx_t *ctx);

void fdt_traverse(void *fdt, fdt_callback_t callback);
uint64_t fdt_translate_address(fdt_traverse_ctx_t *ctx, uint64_t reg_addr);
uint64_t fdt_read_cells(const uint32_t *cells, uint32_t count);

#endif /* _KERNEL_FDT_H */

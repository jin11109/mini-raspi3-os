/**
 * TODO: Unflatten the device tree into an in-memory tree using malloc.
 *
 * Currently, the FDT parser traverses the tree every time it needs to retrieve
 * data, avoiding dynamic memory allocation. This approach is used during early
 * kernel boot.
 */

#include "fdt.h"

#include "string.h"

static inline uint32_t fdt32_to_cpu(uint32_t val) {
    return __builtin_bswap32(val);
}

/* Read 32-bit or 64-bit value based on cell count */
uint64_t fdt_read_cells(const uint32_t *cells, uint32_t count) {
    if (count == 1) return fdt32_to_cpu(cells[0]);
    if (count == 2) {
        return ((uint64_t)fdt32_to_cpu(cells[0]) << 32) |
               fdt32_to_cpu(cells[1]);
    }
    return 0;
}

/* Core: Translate Bus Address to CPU Physical Address */
uint64_t fdt_translate_address(fdt_traverse_ctx_t *ctx, uint64_t reg_addr) {
    uint64_t current_addr = reg_addr;

    /* Traverse upwards from the parent of the current node to the Root */
    for (int i = ctx->depth - 1; i >= 0; i--) {
        /* If no ranges property, or ranges is empty (1:1 mapping) */
        if (ctx->stack[i].ranges_prop == NULL ||
            ctx->stack[i].ranges_len == 0) {
            continue;
        }

        uint32_t child_ac = ctx->stack[i].address_cells;
        uint32_t parent_ac = (i > 0) ? ctx->stack[i - 1].address_cells : 2;
        uint32_t child_sc = ctx->stack[i].size_cells;

        uint32_t tuple_cells = child_ac + parent_ac + child_sc;
        uint32_t tuple_bytes = tuple_cells * 4;

        const uint32_t *ranges = (const uint32_t *)ctx->stack[i].ranges_prop;
        uint32_t ranges_count = ctx->stack[i].ranges_len / tuple_bytes;

        uint64_t translated = current_addr;
        int matched = 0;

        for (uint32_t r = 0; r < ranges_count; r++) {
            const uint32_t *tuple = ranges + (r * tuple_cells);
            uint64_t child_base = fdt_read_cells(tuple, child_ac);
            uint64_t parent_base = fdt_read_cells(tuple + child_ac, parent_ac);
            uint64_t size =
                fdt_read_cells(tuple + child_ac + parent_ac, child_sc);

            if (current_addr >= child_base &&
                current_addr < child_base + size) {
                translated = parent_base + (current_addr - child_base);
                matched = 1;
                break;
            }
        }

        if (matched) {
            current_addr = translated;
        } else {
            /* TODO: Translation failed or out of mapping range; break and
             * return current value */
            break;
        }
    }
    return current_addr;
}

void fdt_traverse(void *fdt, fdt_callback_t callback) {
    fdt_header_t *header = (fdt_header_t *)fdt;
    if (fdt32_to_cpu(header->magic) != FDT_MAGIC) return;

    const uint32_t *struct_ptr =
        (const uint32_t *)((char *)fdt + fdt32_to_cpu(header->off_dt_struct));
    const char *strings_base =
        (const char *)fdt + fdt32_to_cpu(header->off_dt_strings);

    fdt_traverse_ctx_t ctx;
    ctx.depth = -1;

    char path_buf[256];

    while (1) {
        uint32_t token = fdt32_to_cpu(*struct_ptr++);
        switch (token) {
        case FDT_BEGIN_NODE: {
            const char *node_name = (const char *)struct_ptr;

            int len = 0;
            while (((char *)struct_ptr)[len] != '\0') len++;
            struct_ptr =
                (const uint32_t *)(((uintptr_t)struct_ptr + len + 1 + 3) & ~3);

            ctx.depth++;
            if (ctx.depth < MAX_FDT_DEPTH) {
                ctx.stack[ctx.depth].name = node_name;
                /* FDT Standard: Default address_cells=2, size_cells=1 */
                ctx.stack[ctx.depth].address_cells = 2;
                ctx.stack[ctx.depth].size_cells = 1;
                ctx.stack[ctx.depth].ranges_prop = NULL;
                ctx.stack[ctx.depth].ranges_len = 0;
            }
            break;
        }
        case FDT_PROP: {
            uint32_t len = fdt32_to_cpu(*struct_ptr++);
            uint32_t nameoff = fdt32_to_cpu(*struct_ptr++);
            const void *data = struct_ptr;
            const char *prop_name = strings_base + nameoff;

            /* Intercept special properties and update Context state */
            if (ctx.depth >= 0 && ctx.depth < MAX_FDT_DEPTH) {
                if (strcmp(prop_name, "#address-cells") == 0) {
                    ctx.stack[ctx.depth].address_cells =
                        fdt32_to_cpu(*(const uint32_t *)data);
                } else if (strcmp(prop_name, "#size-cells") == 0) {
                    ctx.stack[ctx.depth].size_cells =
                        fdt32_to_cpu(*(const uint32_t *)data);
                } else if (strcmp(prop_name, "ranges") == 0) {
                    ctx.stack[ctx.depth].ranges_prop = data;
                    ctx.stack[ctx.depth].ranges_len = len;
                }
            }

            /* Construct absolute path (without malloc) */
            int pos = 0;
            for (int i = 0; i <= ctx.depth; i++) {
                if (i > 0 && pos > 0 && path_buf[pos - 1] != '/')
                    path_buf[pos++] = '/';
                const char *n = ctx.stack[i].name;
                while (*n && pos < 254) path_buf[pos++] = *n++;
            }
            if (pos == 0) path_buf[pos++] = '/';
            path_buf[pos] = '\0';

            callback(path_buf, prop_name, data, len, &ctx);

            struct_ptr =
                (const uint32_t *)(((uintptr_t)struct_ptr + len + 3) & ~3);
            break;
        }
        case FDT_END_NODE:
            if (ctx.depth >= 0) ctx.depth--;
            break;
        case FDT_NOP:
            break;
        case FDT_END:
            return;
        default:
            return;
        }
    }
}

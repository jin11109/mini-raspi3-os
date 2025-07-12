#include "fdt.h"

#include "malloc.h"
#include "string.h"

char *fdt_build_path_alloc(fdt_traverse_ctx *ctx, const char *name) {
    int total_len = 0;

    for (int i = 0; i < ctx->depth; i++) {
        const char *seg = ctx->path_stack[i];
        while (*seg++) total_len++;
        total_len++; // for '/'
    }
    const char *seg = name;
    while (*seg++) total_len++;
    total_len++; // null terminator

    char *result = (char *)malloc(total_len);
    if (!result) return NULL;

    int pos = 0;
    for (int i = 0; i < ctx->depth; i++) {
        const char *seg = ctx->path_stack[i];
        while (*seg) result[pos++] = *seg++;
        result[pos++] = '/';
    }
    seg = name;
    while (*seg) result[pos++] = *seg++;
    result[pos] = '\0';

    return result;
}

void fdt_traverse(void *fdt, fdt_callback_t callback) {
    fdt_header_t *header = (fdt_header_t *)fdt;
    if (__builtin_bswap32(header->magic) != FDT_MAGIC) return;

    const uint32_t *struct_ptr =
        (const uint32_t *)((char *)fdt +
                           __builtin_bswap32(header->off_dt_struct));
    const char *strings_base =
        (const char *)fdt + __builtin_bswap32(header->off_dt_strings);

    fdt_traverse_ctx ctx;
    ctx.capacity = 8;
    ctx.depth = 0;
    ctx.path_stack = (const char **)malloc(sizeof(char *) * ctx.capacity);
    if (!ctx.path_stack) return;

    while (1) {
        uint32_t token = __builtin_bswap32(*struct_ptr++);
        switch (token) {
            case FDT_BEGIN_NODE: {
                const char *node_name = (const char *)struct_ptr;

                // align to 4-byte boundary after name
                while (*((char *)struct_ptr) != '\0') {
                    struct_ptr = (const uint32_t *)((char *)struct_ptr + 1);
                }
                struct_ptr =
                    (const uint32_t *)(((uintptr_t)struct_ptr + 4) & ~3);

                // expand stack if needed
                if (ctx.depth >= ctx.capacity) {
                    int new_cap = ctx.capacity * 2;
                    const char **new_stack =
                        (const char **)malloc(sizeof(char *) * new_cap);
                    if (!new_stack) return;
                    for (int i = 0; i < ctx.depth; i++)
                        new_stack[i] = ctx.path_stack[i];
                    free(ctx.path_stack);
                    ctx.path_stack = new_stack;
                    ctx.capacity = new_cap;
                }

                ctx.path_stack[ctx.depth++] = node_name;
                break;
            }
            case FDT_PROP: {
                uint32_t len = __builtin_bswap32(*struct_ptr++);
                uint32_t nameoff = __builtin_bswap32(*struct_ptr++);
                const void *data = struct_ptr;
                const char *prop_name = strings_base + nameoff;

                char *path = fdt_build_path_alloc(&ctx, "");
                callback(path, prop_name, data, len);
                free(path);

                struct_ptr =
                    (const uint32_t *)(((uintptr_t)struct_ptr + len + 3) & ~3);
                break;
            }
            case FDT_END_NODE:
                ctx.depth--;
                break;
            case FDT_NOP:
                break;
            case FDT_END:
                free(ctx.path_stack);
                return;
            default:
                free(ctx.path_stack);
                return;
        }
    }
}

#include "bmalloc.h"

#include "def.h"
#include "utils.h"

extern char __bootloader_heap_start, __bootloader_heap_end;

typedef struct mblock {
        size_t size;
        uint64_t is_used;

} mblock_t; // 16 bytes

static char* free_ptr;
static char is_init_bmalloc = 0;

void init_bmalloc() { free_ptr = &__bootloader_heap_start; }

/**
 * Bootloader malloc
 */
void* bmalloc(size_t size) {
    if (!is_init_bmalloc) {
        init_bmalloc();
        is_init_bmalloc = 1;
    }

    if (size == 0) return NULL;

    size = ALIGN(size, 8);
    if (size + sizeof(mblock_t) + free_ptr >= &__bootloader_heap_end) {
        return NULL;
    }

    mblock_t* mblock = (mblock_t*)(free_ptr);
    mblock->is_used = 1;
    mblock->size = size;

    char* new_free_ptr = free_ptr + sizeof(mblock_t);
    free_ptr += size;

    return (void*)new_free_ptr;
}
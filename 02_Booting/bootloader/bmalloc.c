#include "bmalloc.h"

#include "def.h"
#include "utils.h"

typedef struct mblock {
        size_t size;
        uint64_t is_used;

} mblock_t; // 16 bytes

static char* free_ptr = NULL;
static char* heap_end = NULL;
static char is_init_bmalloc = 0;

void init_bmalloc() {
    uintptr_t bootloader_start, reserved_region_start, bootloader_heap_start,
        bootloader_heap_end;
    asm volatile(
        "ldr %0, =__bootloader_start\n"
        "ldr %1, =__reserved_region_start\n"
        "ldr %2, =__bootloader_heap_start\n"
        "ldr %3, =__bootloader_heap_end\n"
        : "=r"(bootloader_start), "=r"(reserved_region_start),
          "=r"(bootloader_heap_start), "=r"(bootloader_heap_end));

    uintptr_t offset = reserved_region_start - bootloader_start;
    free_ptr = (char*)(bootloader_heap_start + offset);
    heap_end = (char*)(bootloader_heap_end + offset);
}

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
    if (size + sizeof(mblock_t) + free_ptr >= heap_end) {
        return NULL;
    }

    mblock_t* mblock = (mblock_t*)(free_ptr);
    mblock->is_used = 1;
    mblock->size = size;

    char* new_free_ptr = free_ptr + sizeof(mblock_t);
    free_ptr += size;

    return (void*)new_free_ptr;
}
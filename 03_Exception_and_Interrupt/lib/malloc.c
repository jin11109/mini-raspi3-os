#include "malloc.h"

#include "def.h"
#include "mm.h"

extern char __kernel_heap_start, __kernel_heap_end;

typedef struct mblock {
        size_t size;
        uint64_t is_used;

} mblock_t; // 16 bytes

static char* free_ptr;
static char is_init_malloc = 0;

size_t align(size_t s, size_t alignment) {
    if (s == 0) return 0;
    if ((s % alignment) == 0) return s;
    return ((s / alignment) + 1) * alignment;
}

void init_malloc() { free_ptr = &__kernel_heap_start; }

void* malloc(size_t size) {
    if (!is_init_malloc) {
        init_malloc();
        is_init_malloc = 1;
    }

    if (size == 0) return NULL;

    size = align(size, 8);
    if (size + sizeof(mblock_t) + free_ptr >= &__kernel_heap_end) {
        return NULL;
    }

    mblock_t* mblock = (mblock_t*)(free_ptr);
    mblock->is_used = 1;
    mblock->size = size;

    char* new_free_ptr = free_ptr + sizeof(mblock_t);
    free_ptr += size;

    return (void*)new_free_ptr;
}

/* TODO: Implement this function */
void* realloc(void* ptr, size_t new_size) {
    void* new_ptr = malloc(new_size);
    size_t old_size = ((mblock_t*)(ptr))->size;
    memcpy(new_ptr, ptr, old_size);
    free(ptr);
    return new_ptr;
}

/* TODO: Implement this function */
void free(void* addr) {}
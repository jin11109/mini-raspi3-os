#ifndef _MM_H
#define _MM_H

#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define LOW_MEMORY (2 * SECTION_SIZE)

#include "def.h"

void memzero(void* src, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);
void memcpy(void *dst, const void *src, size_t n);

#endif /*_MM_H */

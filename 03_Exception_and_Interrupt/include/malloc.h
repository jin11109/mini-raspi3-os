#ifndef _MALLOC_H
#define _MALLOC_H

#include "def.h"

void* malloc(size_t size);
void* realloc(void* ptr, size_t new_size);
void free(void* addr);

#endif /*_MALLOC_H*/
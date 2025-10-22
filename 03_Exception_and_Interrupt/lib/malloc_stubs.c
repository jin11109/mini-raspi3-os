#include "def.h"

__attribute__((weak)) void* malloc(size_t size) { return NULL; };
__attribute__((weak)) void free(void* addr) {};
__attribute__((weak)) void init_malloc() {};
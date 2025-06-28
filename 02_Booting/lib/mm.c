#include "def.h"

void memzero(void *src, size_t n) {
    uint8_t *p = (uint8_t *)src;
    for (size_t i = 0; i < n; i++) {
        p[i] = 0;
    }
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }
    return 0;
}

void memcpy(const void *src, void *dst, size_t n) {
    const uint8_t *p1 = (const uint8_t *)src;
    uint8_t *p2 = (uint8_t *)dst;

    for (size_t i = 0; i < n; i++) {
        p2[i] = p1[i];
    }
}
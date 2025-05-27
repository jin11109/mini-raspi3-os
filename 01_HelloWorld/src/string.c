#include <string.h>

int strcmp(const char* str1, const char* str2) {
    /* TODO : This error handling is a temporarily way.*/
    if (str1 == NULL || str2 == NULL) {
        return 2;
    }

    while (*str1 == *str2) {
        if (*str1 == '\0') {
            return 0;
        }
        str1++;
        str2++;
    }
    if (*str1 > *str2) {
        return 1;
    } else {
        return -1;
    }
}
#include "string.h"

#include "def.h"

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

size_t strlen(char* str) {
    /* TODO : This error handling should be implemented. */
    if (str == NULL) {
    }

    size_t len = 0;
    while (*str != '\0') {
        len++;
        str++;
    }
    return len;
}

void strrev(char* str) {
    if (str == NULL) {
        return;
    }

    size_t len = strlen(str);
    for (size_t i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

void itoa_dec(int32_t num, char* buf) {
    /* TODO : This error handling should be implemented.*/
    if (buf == NULL) {
    }

    int pos = 0;
    int8_t sign = 0;
    if (num < 0) {
        sign = -1;
        num = -num;
    }
    do {
        buf[pos++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);

    if (sign < 0) {
        buf[pos++] = '-';
    }
    buf[pos] = '\0';
    strrev(buf);
}

void itoa_dec64(int64_t num, char* buf) {
    /* TODO : This error handling should be implemented.*/
    if (buf == NULL) {
    }

    int pos = 0;
    int8_t sign = 0;
    if (num < 0) {
        sign = -1;
        num = -num;
    }
    do {
        buf[pos++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);

    if (sign < 0) {
        buf[pos++] = '-';
    }
    buf[pos] = '\0';
    strrev(buf);
}

void utoa_dec(uint32_t num, char* buf) {
    /* TODO : This error handling should be implemented.*/
    if (buf == NULL) {
    }

    int pos = 0;
    do {
        buf[pos++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);

    buf[pos] = '\0';
    strrev(buf);
}

void utoa_dec64(uint64_t num, char* buf) {
    /* TODO : This error handling should be implemented.*/
    if (buf == NULL) {
    }

    int pos = 0;
    do {
        buf[pos++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);

    buf[pos] = '\0';
    strrev(buf);
}

void utoa_hex(uint32_t num, char* buf) {
    /* TODO : This error handling should be implemented.*/
    if (buf == NULL) {
    }

    const char* hex = "0123456789abcdef";
    int pos = 0;

    do {
        buf[pos++] = hex[num % 16];
        num /= 16;
    } while (num > 0);

    buf[pos] = '\0';
    strrev(buf);
}

void utoa_hex64(uint64_t num, char* buf) {
    /* TODO : This error handling should be implemented.*/
    if (buf == NULL) {
    }

    const char* hex = "0123456789abcdef";
    int pos = 0;

    do {
        buf[pos++] = hex[num % 16];
        num /= 16;
    } while (num > 0);

    buf[pos] = '\0';
    strrev(buf);
}

/* Decimal string to signed int32 */
int32_t atoi_dec(const char* str, size_t len) {
    if (str == NULL) return 0;

    int32_t result = 0;
    int8_t negative = 0;
    uintptr_t end = (size_t)(str) + len;

    if (*str == '-') {
        negative = 1;
        str++;
    }
    while ((uintptr_t)(str) < end) {
        if (!(*str >= '0' && *str <= '9')) {
            /* TODO : This error handling should be implemented.*/
            break;
        }
        result = result * 10 + (*str - '0');
        str++;
    }


    return negative ? -result : result;
}

/* Decimal string to signed int64 */
int64_t atoi_dec64(const char* str, size_t len) {
    if (str == NULL) return 0;

    int64_t result = 0;
    int8_t negative = 0;
    uintptr_t end = (size_t)(str) + len;

    if (*str == '-') {
        negative = 1;
        str++;
    }
    while ((uintptr_t)(str) < end) {
        if (!(*str >= '0' && *str <= '9')) {
            /* TODO : This error handling should be implemented.*/
            break;
        }
        result = result * 10 + (*str - '0');
        str++;
    }

    return negative ? -result : result;
}

/* Decimal string to unsigned int32 */
uint32_t atou_dec(const char* str, size_t len) {
    if (str == NULL) return 0;

    uint32_t result = 0;
    uintptr_t end = (size_t)(str) + len;

    while ((uintptr_t)(str) < end) {
        if (!(*str >= '0' && *str <= '9')) {
            /* TODO : This error handling should be implemented.*/
            break;
        }
        result = result * 10 + (*str - '0');
        str++;
    }

    return result;
}

/* Decimal string to unsigned int64 */
uint64_t atou_dec64(const char* str, size_t len) {
    if (str == NULL) return 0;

    uint64_t result = 0;
    uintptr_t end = (size_t)(str) + len;

    while ((uintptr_t)(str) < end) {
        if (!(*str >= '0' && *str <= '9')) {
            /* TODO : This error handling should be implemented.*/
            break;
        }
        result = result * 10 + (*str - '0');
        str++;
    }

    return result;
}

/* Hex string (lower or upper case) to uint32_t */
uint32_t atou_hex(const char* str, size_t len) {
    if (str == NULL) return 0;

    uint32_t result = 0;
    uintptr_t end = (size_t)(str) + len;

    while ((uintptr_t)(str) < end) {
        if (!((*str >= '0' && *str <= '9') || (*str >= 'a' && *str <= 'f') ||
              (*str >= 'A' && *str <= 'F'))) {
            /* TODO : This error handling should be implemented.*/
            break;
        }
        result *= 16;
        if (*str >= '0' && *str <= '9') {
            result += *str - '0';
        } else if (*str >= 'a' && *str <= 'f') {
            result += *str - 'a' + 10;
        } else {
            result += *str - 'A' + 10;
        }
        str++;
    }

    return result;
}

/* Hex string (lower or upper case) to uint64_t */
uint64_t atou_hex64(const char* str, size_t len) {
    if (str == NULL) return 0;

    uint64_t result = 0;
    uintptr_t end = (size_t)(str) + len;

    while ((uintptr_t)(str) < end) {
        if (!((*str >= '0' && *str <= '9') || (*str >= 'a' && *str <= 'f') ||
              (*str >= 'A' && *str <= 'F'))) {
            /* TODO : This error handling should be implemented.*/
            break;
        }
        result *= 16;
        if (*str >= '0' && *str <= '9') {
            result += *str - '0';
        } else if (*str >= 'a' && *str <= 'f') {
            result += *str - 'a' + 10;
        } else {
            result += *str - 'A' + 10;
        }
        str++;
    }

    return result;
}
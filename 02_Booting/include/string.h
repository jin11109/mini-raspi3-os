#ifndef _STRING_H
#define _STRING_H
#include "def.h"

int strcmp(const char* str1, const char* str2);

size_t strlen(char* str);
void strrev(char* str);
void itoa_dec(int32_t num, char* buf);
void itoa_dec64(int64_t num, char* buf);
void utoa_dec(uint32_t num, char* buf);
void utoa_dec64(uint64_t num, char* buf);
void utoa_hex(uint32_t num, char* buf);
void utoa_hex64(uint64_t num, char* buf);

#endif /*_STRING_H */
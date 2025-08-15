#ifndef _STRING_H
#define _STRING_H
#include "def.h"

int strcmp(const char* str1, const char* str2);

size_t strlen(char* str);
void strrev(char* str);
void strncpy(char* dst, const char* src, size_t len);
size_t split_inplace(char** buf, char* str, char delim, size_t max_substr);
size_t count_substr(const char* str, char delim, size_t len);

void itoa_dec(int32_t num, char* buf);
void itoa_dec64(int64_t num, char* buf);
void utoa_dec(uint32_t num, char* buf);
void utoa_dec64(uint64_t num, char* buf);
void utoa_hex(uint32_t num, char* buf);
void utoa_hex64(uint64_t num, char* buf);

int32_t atoi_dec(const char* str, size_t len);
int64_t atoi_dec64(const char* str, size_t len);
uint32_t atou_dec(const char* str, size_t len);
uint64_t atou_dec64(const char* str, size_t len);
uint32_t atou_hex(const char* str, size_t len);
uint64_t atou_hex64(const char* str, size_t len);

#endif /*_STRING_H */
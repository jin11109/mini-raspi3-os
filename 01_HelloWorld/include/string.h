#ifndef _STRING_H
#define _STRING_H
#include "def.h"

int strcmp(const char* str1, const char* str2);

size_t strlen(char* str);
void strrev(char* str);
void itoa_dec(int num, char* buf);
void utoa_dec(unsigned int num, char* buf);
void utoa_hex(unsigned int num, char* buf);

#endif /*_STRING_H */
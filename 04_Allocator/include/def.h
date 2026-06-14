#ifndef _INCLUDE_DEF_H
#define _INCLUDE_DEF_H

#define NULL ((void *)0)

#define offsetof(TYPE, MEMBER) ((size_t)&((TYPE *)0)->MEMBER)

typedef unsigned long uintptr_t;
typedef long intptr_t;

typedef unsigned char uint8_t;
typedef char int8_t;
typedef short unsigned int uint16_t;
typedef short int int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned long uint64_t;
typedef long int64_t;

typedef unsigned long size_t;

#ifndef __cplusplus
#define bool _Bool
#define true 1
#define false 0
#endif

#endif /* _INCLUDE_DEF_H */

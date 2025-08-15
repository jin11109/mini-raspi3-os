#ifndef _UTILS_H
#define _UTILS_H

extern void delay(unsigned long);
extern void put32(unsigned long, unsigned int);
extern unsigned int get32(unsigned long);

void printf(const char* fmt, ...);
char getchar();

/* The parameter 'alignment' should be a power of 2 */
#define ALIGN(s, alignment) (((s) + (alignment - 1)) & ~(alignment - 1))

#endif /*_UTILS_H */

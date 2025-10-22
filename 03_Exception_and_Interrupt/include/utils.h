#ifndef _UTILS_H
#define _UTILS_H

/* Memory barrier */
// system barrier : ensure write/read op order
#define mb asm volatile("dmb sy" ::: "memory")
// Write barrier : ensure write op order
#define wmb asm volatile("dmb st" ::: "memory")
// Read barrier : ensure read op order
#define rmb asm volatile("dmb ld" ::: "memory")
// Data synchronization barrier : ensure memory access
#define dsb asm volatile("dsb sy" ::: "memory")
// Instruction synchronization barrier : ensure lastest instruction in pipline
#define isb asm volatile("isb" ::: "memory")

#ifndef DEBUG
#define MMIO_READ32(addr) *((volatile uint32_t*)(addr))
#define MMIO_WRITE32(val, addr) (*((volatile uint32_t*)(addr)) = (val))
#define MMIO_WRITE32_AND(val, addr) (*((volatile uint32_t*)(addr)) &= (val))
#define MMIO_WRITE32_OR(val, addr) (*((volatile uint32_t*)(addr)) |= (val))
#endif

#ifdef DEBUG
#define MMIO_READ32(addr)                    \
    ({                                       \
        uint32_t val;                        \
        dsb;                                 \
        val = *((volatile uint32_t*)(addr)); \
        dsb;                                 \
        val;                                 \
    })

#define MMIO_WRITE32(val, addr)                \
    do {                                       \
        dsb;                                   \
        *((volatile uint32_t*)(addr)) = (val); \
        dsb;                                   \
    } while (0)

#define MMIO_WRITE32_AND(val, addr)               \
    do {                                          \
        dsb;                                      \
        (*((volatile uint32_t*)(addr)) &= (val)); \
        dsb;                                      \
    } while (0)

#define MMIO_WRITE32_OR(val, addr)                \
    do {                                          \
        dsb;                                      \
        (*((volatile uint32_t*)(addr)) |= (val)); \
        dsb;                                      \
    } while (0)

#endif

/* TODO : Remove this */
extern void delay(unsigned long);
extern void put32(unsigned long, unsigned int);
extern unsigned int get32(unsigned long);

#ifndef BOOTLOADER
void printf(const char* fmt, ...);
#endif
void printf_sync(const char* fmt, ...);
char getchar();
char getchar_sync();

/* The parameter 'alignment' should be a power of 2 */
#define ALIGN(s, alignment) (((s) + (alignment - 1)) & ~(alignment - 1))

#endif /*_UTILS_H */

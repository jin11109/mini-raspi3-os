#ifndef _CPIO_H
#define _CPIO_H

#include "def.h"

#define CPIO_NEWC_MAGIC "070701"

struct cpio_newc_header {
        char c_magic[6];
        char c_ino[8];
        char c_mode[8];
        char c_uid[8];
        char c_gid[8];
        char c_nlink[8];
        char c_mtime[8];
        char c_filesize[8];
        char c_devmajor[8];
        char c_devminor[8];
        char c_rdevmajor[8];
        char c_rdevminor[8];
        char c_namesize[8];
        char c_check[8];
};

typedef struct {
        const char* name;
        const void* data;
        uint32_t data_size;
} cpio_entry_t;

#define CPIO_FIELD_OFFSET(field) \
    ((size_t)&(((struct cpio_newc_header*)0)->field))
#define CPIO_HEADER_SIZE sizeof(struct cpio_newc_header)

extern cpio_entry_t** cpio_entry;
extern size_t cpio_entry_len;
extern uint64_t initramfs_start;
extern uint64_t initramfs_end;

void init_cpio();
void* cpio_get_executable_file(const char* name);

#endif /* _CPIO_H */

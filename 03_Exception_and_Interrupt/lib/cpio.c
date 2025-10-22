#include "cpio.h"

#include "command_registry.h"
#include "def.h"
#include "malloc.h"
#include "mm.h"
#include "string.h"
#include "utils.h"

/* file type mask */
#define CPIO_MODE_IFMT 0xF000  // file type mask
#define CPIO_MODE_IFREG 0x8000 // regular file
#define CPIO_MODE_IFDIR 0x4000 // directory

/* Execute bits */
#define CPIO_MODE_IXUSR 0x0040 // owner execute
#define CPIO_MODE_IXGRP 0x0008 // group execute
#define CPIO_MODE_IXOTH 0x0001 // other execute

/* Convenience: any execute bit */
#define CPIO_MODE_EXEC_ANY (CPIO_MODE_IXUSR | CPIO_MODE_IXGRP | CPIO_MODE_IXOTH)

cpio_entry_t** cpio_entry = NULL;
size_t cpio_entry_len = 0;
uint64_t initramfs_start;
uint64_t initramfs_end;

void init_cpio() {
    const char* ptr = (char*)initramfs_start;

    uint64_t capacity = 10;
    cpio_entry = malloc(capacity * sizeof(cpio_entry_t));
    cpio_entry_t* entry_ptr = *cpio_entry;

    while (1) {
        const struct cpio_newc_header* hdr =
            (const struct cpio_newc_header*)ptr;

        if (memcmp(hdr->c_magic, CPIO_NEWC_MAGIC, 6) != 0) {
            printf("Invalid CPIO magic %c%c%c%c%c%c\r\n", hdr->c_magic[0],
                   hdr->c_magic[1], hdr->c_magic[2], hdr->c_magic[3],
                   hdr->c_magic[4], hdr->c_magic[5]);
            return;
        }

        uint32_t namesize = atou_hex(hdr->c_namesize, 8);
        uint32_t filesize = atou_hex(hdr->c_filesize, 8);

        const char* name = ptr + CPIO_HEADER_SIZE;
        const char* file_data =
            (const char*)ALIGN((uintptr_t)(name + namesize), 4);

        if (strcmp(name, "TRAILER!!!") == 0) break;

        if (cpio_entry_len >= capacity) {
            capacity <<= 1;
            if (realloc(cpio_entry, capacity * sizeof(cpio_entry_t)) == NULL) {
                /* TODO : This error handling should be implemented.*/
                return;
            }
        }
        cpio_entry_len++;
        entry_ptr->name = name;
        entry_ptr->data = file_data;
        entry_ptr->data_size = filesize;
        entry_ptr++;

        ptr = (const char*)ALIGN((uintptr_t)(file_data + filesize), 4);
    }
}

void* cpio_get_executable_file(const char* name) {
    cpio_entry_t* ptr = *cpio_entry;
    for (size_t i = 0; i < cpio_entry_len; i++) {
        if (strcmp(name, ptr->name) == 0) {
            /* Test if executable */
            const struct cpio_newc_header* hdr =
                (const struct cpio_newc_header*)(ptr->name - CPIO_HEADER_SIZE);
            uint32_t c_mode = atou_hex(hdr->c_mode, 8);
            uint32_t type = c_mode & CPIO_MODE_IFMT;

            if (type == CPIO_MODE_IFDIR) {
                printf("This is a directory\r\n");
                return NULL;
            }

            if (type == CPIO_MODE_IFREG) {
                if (c_mode & CPIO_MODE_EXEC_ANY) {
                    return (void*)ptr->data;
                }
            }

            printf("This is not executable file\r\n");
            return NULL;
        }
        ptr++;
    }
    return NULL;
}

void cmd_ls(int argc, char** argv) {
    cpio_entry_t* ptr = *cpio_entry;
    for (size_t i = 0; i < cpio_entry_len; i++) {
        printf("%s\r\n", ptr->name);
        ptr++;
    }
}
COMMAND_DEFINE("ls", cmd_ls);

void cmd_cat(int argc, char** argv) {
    if (argc != 2) {
        return;
    }
    cpio_entry_t* ptr = *cpio_entry;
    for (size_t i = 0; i < cpio_entry_len; i++) {
        if (strcmp(argv[1], ptr->name) == 0) {
            const char* data_ptr = ptr->data;
            for (size_t j = 0; j < ptr->data_size; j++) {
                printf("%c", *data_ptr);
                data_ptr++;
            }
            printf("\r\n");
            return;
        }
        ptr++;
    }
}
COMMAND_DEFINE("cat", cmd_cat);
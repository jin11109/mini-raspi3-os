#include "cpio.h"

#include "command_registry.h"
#include "def.h"
#include "malloc.h"
#include "mm.h"
#include "string.h"
#include "utils.h"

cpio_entry_t **cpio_entry = NULL;
size_t cpio_entry_len = 0;

void init_cpio(const char *cpio_base) {
    const char *ptr = cpio_base;

    uint64_t capacity = 10;
    cpio_entry = malloc(capacity * sizeof(cpio_entry_t));
    cpio_entry_t *entry_ptr = *cpio_entry;

    while (1) {
        const struct cpio_newc_header *hdr =
            (const struct cpio_newc_header *)ptr;

        if (memcmp(hdr->c_magic, CPIO_NEWC_MAGIC, 6) != 0) {
            printf("Invalid CPIO magic %c%c%c%c%c%c\r\n", hdr->c_magic[0],
                   hdr->c_magic[1], hdr->c_magic[2], hdr->c_magic[3],
                   hdr->c_magic[4], hdr->c_magic[5]);
            return;
        }

        uint32_t namesize = atou_hex(hdr->c_namesize, 8);
        uint32_t filesize = atou_hex(hdr->c_filesize, 8);

        const char *name = ptr + CPIO_HEADER_SIZE;
        const char *file_data =
            (const char *)ALIGN((uintptr_t)(name + namesize), 4);

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

        ptr = (const char *)ALIGN((uintptr_t)(file_data + filesize), 4);
    }
}

void cmd_ls(int argc, char **argv) {
    if (cpio_entry == NULL) {
        init_cpio((const char *)(0x20000000));
    }
    cpio_entry_t *ptr = *cpio_entry;
    for (size_t i = 0; i < cpio_entry_len; i++) {
        printf("%s\r\n", ptr->name);
        ptr++;
    }
}
COMMAND_DEFINE("ls", cmd_ls);

void cmd_cat(int argc, char **argv) {
    if (argc != 2) {
        return;
    }
    cpio_entry_t *ptr = *cpio_entry;
    for (size_t i = 0; i < cpio_entry_len; i++) {
        if (strcmp(argv[1], ptr->name) == 0) {
            const char *data_ptr = ptr->data;
            for (size_t j = 0; j < ptr->data_size; j++) {
                printf("%c", *data_ptr);
                data_ptr++;
            }
            printf("\r\n");
        }
        ptr++;
    }
}
COMMAND_DEFINE("cat", cmd_cat);
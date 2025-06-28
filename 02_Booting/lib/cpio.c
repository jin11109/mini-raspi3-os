#include "cpio.h"

#include "def.h"
#include "mm.h"
#include "string.h"
#include "utils.h"

void parse_cpio(const char *cpio_base) {
    const char *ptr = cpio_base;

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

        printf("File: %s \r\n", name);
        printf("Data:\r\n");

        const char *data_ptr = file_data;
        for (size_t i = 0; i < filesize; i++) {
            printf("%c", *data_ptr);
            data_ptr++;
        }

        printf("\r\n\r\n");
        ptr = (const char *)ALIGN((uintptr_t)(file_data + filesize), 4);
    }
}

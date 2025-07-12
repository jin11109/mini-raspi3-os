#include "bmalloc.h"
#include "crc.h"
#include "def.h"
#include "mini_uart.h"
#include "power.h"
#include "utils.h"
#define KERNEL_ADDR (0xa0000)
#define OTA_KERNEL_ENTRY ((void (*)(uint64_t, uint64_t, uint64_t))0x200000)
#define KERNEL_ENTRY ((void (*)(uint64_t, uint64_t, uint64_t))KERNEL_ADDR)
#define ACK 0x06
#define NACK 0x15
#define TIMEOUT 1000
#define MAGIC_HEADER "IMGX"

size_t zrle_decompress(const uint8_t *in, size_t in_len, uint8_t *out) {
    size_t i = 0, j = 0;
    while (i < in_len) {
        if (i + 2 < in_len && in[i] == 0x00 && in[i + 1] == 0x00) {
            uint8_t count = in[i + 2];
            for (uint8_t k = 0; k < count; ++k) {
                out[j++] = 0x00;
            }
            i += 3;
        } else {
            out[j++] = in[i++];
        }
    }
    return j; // decompressed size
}

int load_kernel_from_uart() {
    // Receive magic header
    uint8_t magic[4];
    int tries = 0;
    while (tries++ < TIMEOUT) {
        magic[0] = magic[1];
        magic[1] = magic[2];
        magic[2] = magic[3];
        magic[3] = getchar();

        if (magic[0] == 'I' && magic[1] == 'M' && magic[2] == 'G' &&
            magic[3] == 'X')
            break;
    }
    if (magic[0] != 'I' || magic[1] != 'M' || magic[2] != 'G' ||
        magic[3] != 'X') {
        printf("%c", NACK);
        printf("Invalid header\r\n");
        return 0;
    }

    // Receive length
    uint32_t len = 0;
    for (int i = 0; i < 4; i++) {
        len |= ((uint32_t)getchar()) << (8 * i);
    }

    // printf("Receiving %d bytes\r\n", len);

    // Receive payload
    uint8_t *recv_buf = (uint8_t *)bmalloc(len + 1);
    if (recv_buf == NULL) {
        printf("%c", NACK);
        printf("Heap error\r\n");
        return 0;
    }
    for (uint32_t i = 0; i < len; i++) {
        recv_buf[i] = getchar();
    }

    // Receive CRC
    unsigned int recv_crc = 0;
    for (int i = 0; i < 4; ++i) {
        recv_crc |= ((unsigned int)getchar()) << (8 * i);
    }

    // Check CRC
    unsigned int calc_crc = crc32_calculate(recv_buf, len);
    if (calc_crc == recv_crc) {
        uint8_t *dst = (uint8_t *)(0x200000);
        size_t decompressed_size = zrle_decompress(recv_buf, len, dst);
        printf("%c", ACK);
        printf("Kernel received, received %ld bytes, decompressed size %ld\r\n",
               len, decompressed_size);
        return 1;
    } else {
        printf("%c", NACK);
        printf("Payload is wrong\r\n");
        return 0;
    }
}

void bootloader_main(uint64_t dtb_addr, uint64_t x1, uint64_t x2) {
    cancel_reboot();
    uart_init();

    printf(
        "Bootloader Start\r\n"
        "[1] Load Kernel from mini UART\r\n"
        "[2] Boot from existing Kernel\r\n");

    do {
        printf("Enter choice: ");
        char c = getchar();
        printf("%c\r\n", c);

        if (c == '1') {
            printf("Loading kernel from mini UART...\r\n");
            if (load_kernel_from_uart()) {
                printf("Jumping to kernel...\r\n");
                asm volatile("dsb sy");
                asm volatile("isb");
                OTA_KERNEL_ENTRY(dtb_addr, x1, x2);
            } else {
                printf("Please try again");
            }
        } else if (c == '2') {
            printf("Booting existing kernel...\r\n");
            asm volatile("dsb sy");
            asm volatile("isb");
            KERNEL_ENTRY(dtb_addr, x1, x2);
        } else {
            printf("Error choice, please enter again\r\n");
        }
    } while (1);
}
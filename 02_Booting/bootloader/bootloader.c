#include "crc.h"
#include "def.h"
#include "mini_uart.h"
#include "utils.h"
#define KERNEL_ADDR (0x90000)
#define KERNEL_MAIN ((void (*)(void))0x90000)
#define ACK 0x06
#define NACK 0x15
#define TIMEOUT 1000
#define MAGIC_HEADER "IMGX"

extern void kernel_main(void);

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
        printf("Invalid header\r\n");
        return -1;
    }

    // Receive length
    uint32_t len = 0;
    for (int i = 0; i < 4; i++) {
        len |= ((uint32_t)getchar()) << (8 * i);
    }

    // printf("Receiving %d bytes\r\n", len);

    // Receive payload
    uint8_t *dst = (uint8_t *)KERNEL_ADDR;
    for (uint32_t i = 0; i < len; i++) {
        dst[i] = getchar();
    }

    // Receive CRC
    unsigned int recv_crc = 0;
    for (int i = 0; i < 4; ++i) {
        recv_crc |= ((unsigned int)uart_recv()) << (8 * i);
    }

    // Check CRC
    unsigned int calc_crc = crc32_calculate(dst, len);

    if (calc_crc == recv_crc) {
        printf("%c", ACK);
        printf("Kernel received, received %d bytes\r\n", len);
        return 1;
    } else {
        dst = (uint8_t *)KERNEL_ADDR;
        for (uint32_t i = 0; i < len; i++) {
            dst[i] = 0;
        }
        printf("%c", NACK);
        printf("Payload is wrong\r\n");
        return -1;
    }
}

void bootloader_main() {
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
                KERNEL_MAIN();
            } else {
                printf("Please try again");
            }
        } else if (c == '2') {
            printf("Booting existing kernel...\r\n");
            kernel_main();
        } else {
            printf("Error choice, please enter again\r\n");
        }
    } while (1);
}
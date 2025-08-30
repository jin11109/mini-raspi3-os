#include "bmalloc.h"
#include "crc.h"
#include "def.h"
#include "mini_uart.h"
#include "mm.h"
#include "power.h"
#include "utils.h"
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

int load_kernel_from_uart(void *kernel_start) {
    // Receive magic header
    uint8_t magic[4];
    int tries = 0;
    while (tries++ < TIMEOUT) {
        magic[0] = magic[1];
        magic[1] = magic[2];
        magic[2] = magic[3];
        magic[3] = getchar_sync();

        if (magic[0] == 'I' && magic[1] == 'M' && magic[2] == 'G' &&
            magic[3] == 'X')
            break;
    }
    if (magic[0] != 'I' || magic[1] != 'M' || magic[2] != 'G' ||
        magic[3] != 'X') {
        printf_sync("%c", NACK);
        printf_sync("Invalid header\r\n");
        return 0;
    }

    // Receive length
    uint32_t len = 0;
    for (int i = 0; i < 4; i++) {
        len |= ((uint32_t)getchar_sync()) << (8 * i);
    }

    // printf_sync("Receiving %d bytes\r\n", len);

    // Receive payload
    uint8_t *recv_buf = (uint8_t *)bmalloc(len + 1);
    if (recv_buf == NULL) {
        printf_sync("%c", NACK);
        printf_sync("Heap error\r\n");
        return 0;
    }
    for (uint32_t i = 0; i < len; i++) {
        recv_buf[i] = getchar_sync();
    }

    // Receive CRC
    unsigned int recv_crc = 0;
    for (int i = 0; i < 4; ++i) {
        recv_crc |= ((unsigned int)getchar_sync()) << (8 * i);
    }

    // Check CRC
    unsigned int calc_crc = crc32_calculate(recv_buf, len);
    if (calc_crc == recv_crc) {
        uint8_t *dst = (uint8_t *)(kernel_start);
        size_t decompressed_size = zrle_decompress(recv_buf, len, dst);
        printf_sync("%c", ACK);
        printf_sync(
            "Kernel received, received %ld bytes, decompressed size %ld\r\n",
            len, decompressed_size);
        return 1;
    } else {
        printf_sync("%c", NACK);
        printf_sync("Payload is wrong\r\n");
        return 0;
    }
}
void bootloader_main(uint64_t dtb_addr, uint64_t x1, uint64_t x2) {
    uintptr_t kernel_start, kernel_blob_start, kernel_blob_end;
    asm volatile(
        "ldr %0, =__kernel_start\n"
        "ldr %1, =__kernel_blob_start\n"
        "ldr %2, =__kernel_blob_end\n"
        : "=r"(kernel_start), "=r"(kernel_blob_start), "=r"(kernel_blob_end));

    void (*kernel_entry)(uint64_t, uint64_t, uint64_t) = (void *)kernel_start;

    cancel_reboot();
    mini_uart_init();

#ifdef DEBUG
    printf_sync(
        "kernel_start,  0x%lx\r\n"
        "kernel_blob_start kernel_blob_end 0x%lx, 0x%lx\r\n",
        kernel_start, kernel_blob_start, kernel_blob_end);

    uintptr_t bootloader_start, bootloader_end, reserved_region_start,
        reserved_region_end, bootloader_heap_start, bootloader_heap_end;
    asm volatile(
        "ldr %0, =__bootloader_start\n"
        "ldr %1, =__bootloader_end\n"
        "ldr %2, =__reserved_region_start\n"
        "ldr %3, =__reserved_region_end\n"
        "ldr %4, =__bootloader_heap_start\n"
        "ldr %5, =__bootloader_heap_end\n"
        : "=r"(bootloader_start), "=r"(bootloader_end),
          "=r"(reserved_region_start), "=r"(reserved_region_end),
          "=r"(bootloader_heap_start), "=r"(bootloader_heap_end));

    printf_sync(
        "__bootloader_start, __bootloader_end = 0x%lx, 0x%lx\r\n"
        "__reserved_region_start, __reserved_region_end = 0x%lx, 0x%lx\r\n"
        "__bootloader_heap_start, __bootloader_heap_end = 0x%lx, 0x%lx\r\n",
        bootloader_start, bootloader_end, reserved_region_start,
        reserved_region_end, bootloader_heap_start, bootloader_heap_end);

#endif

    printf_sync(
        "Bootloader Start\r\n"
        "[1] Load Kernel from mini UART\r\n"
        "[2] Boot from existing Kernel\r\n");

    do {
        printf_sync("Enter choice: ");
        char c = getchar_sync();
        printf_sync("%c\r\n", c);

        if (c == '1') {
            printf_sync("Loading kernel from mini UART...\r\n");
            if (load_kernel_from_uart((void *)kernel_start)) {
                printf_sync("Jumping to kernel...\r\n");
                asm volatile("dsb sy");
                asm volatile("isb");
                kernel_entry(dtb_addr, x1, x2);
            } else {
                printf_sync("Please try again");
            }
        } else if (c == '2') {
            /* relocate_kernel_blob */
            uint64_t size = kernel_blob_end - kernel_blob_start;
            memcpy((void *)kernel_start, (void *)kernel_blob_start, size);

            printf_sync("Booting existing kernel...\r\n");
            asm volatile("dsb sy");
            asm volatile("isb");
            kernel_entry(dtb_addr, x1, x2);
        } else {
            printf_sync("Error choice, please enter again\r\n");
        }
    } while (1);
}
#include "utils.h"

#include "def.h"
#include "mini_uart.h"
#include "string.h"

/*
 * This function provides simple output through the mini UART. Ensure that the
 * number of arguments does not exceed 7.
 */
void printf(const char *fmt, ...) {
    uint64_t args[7];

    __asm__ volatile(
        "mov %[a0], x1 \n"
        "mov %[a1], x2 \n"
        "mov %[a2], x3 \n"
        "mov %[a3], x4 \n"
        "mov %[a4], x5 \n"
        "mov %[a5], x6 \n"
        "mov %[a6], x7 \n"
        : [a0] "=r"(args[0]), [a1] "=r"(args[1]), [a2] "=r"(args[2]),
          [a3] "=r"(args[3]), [a4] "=r"(args[4]), [a5] "=r"(args[5]),
          [a6] "=r"(args[6])
        :
        : "memory");

    int arg_idx = 0;
    char buf[64];

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd':
                    if (arg_idx < 7) {
                        itoa_dec((int)args[arg_idx++], buf);
                        uart_send_string(buf);
                    }
                    break;
                case 'x':
                    if (arg_idx < 7) {
                        utoa_hex((unsigned int)args[arg_idx++], buf);
                        uart_send_string(buf);
                    }
                    break;
                case 'c':
                    if (arg_idx < 7) {
                        uart_send((char)args[arg_idx++]);
                    }
                    break;
                case 'u':
                    if (arg_idx < 7) {
                        utoa_dec((unsigned int)args[arg_idx++], buf);
                        uart_send_string(buf);
                    }
                    break;
                case '%':
                    uart_send('%');
                    break;
                default:
                    uart_send('%');
                    uart_send(*fmt);
                    break;
            }
        } else {
            uart_send(*fmt);
        }
        fmt++;
    }
}

char getchar() {
    return uart_recv();
}
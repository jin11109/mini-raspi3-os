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
        "mov %0, x1\n"
        "mov %1, x2\n"
        "mov %2, x3\n"
        "mov %3, x4\n"
        "mov %4, x5\n"
        "mov %5, x6\n"
        "mov %6, x7\n"
        : "=r"(args[0]), "=r"(args[1]), "=r"(args[2]),
          "=r"(args[3]), "=r"(args[4]), "=r"(args[5]),
          "=r"(args[6])
        :
        : "x1", "x2", "x3", "x4", "x5", "x6", "x7", "memory");

    int arg_idx = 0;
    char buf[64];

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            int is_long = 0;

            if (*fmt == 'l') {
                is_long = 1;
                fmt++;
            }

            if (arg_idx >= 7) {
                /* TODO : This error handling should be implemented. */
                return;
            }

            switch (*fmt) {
                case 'd':
                    if (is_long)
                        itoa_dec64((int64_t)args[arg_idx++], buf);
                    else
                        itoa_dec((int32_t)args[arg_idx++], buf);
                    uart_send_string(buf);
                    break;

                case 'u':
                    if (is_long)
                        utoa_dec64((uint64_t)args[arg_idx++], buf);
                    else
                        utoa_dec((uint32_t)args[arg_idx++], buf);
                    uart_send_string(buf);
                    break;

                case 'x':
                    if (is_long)
                        utoa_hex64((uint64_t)args[arg_idx++], buf);
                    else
                        utoa_hex((uint32_t)args[arg_idx++], buf);
                    uart_send_string(buf);
                    break;

                case 'c':
                    uart_send((char)args[arg_idx++]);
                    break;

                case 's':
                    uart_send_string((char *)args[arg_idx++]);
                    break;

                case '%':
                    uart_send('%');
                    break;

                default:
                    uart_send('%');
                    if (is_long) uart_send('l');
                    uart_send(*fmt);
                    break;
            }
        } else {
            uart_send(*fmt);
        }
        fmt++;
    }
}

char getchar() { return uart_recv(); }
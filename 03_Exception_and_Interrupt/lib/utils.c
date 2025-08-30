#include "utils.h"

#include "def.h"
#include "mini_uart.h"
#include "string.h"

typedef void (*putchar_func_t)(const char);
typedef void (*sendstr_func_t)(const char *);

static void save_args(uint64_t *args) {
    __asm__ volatile(
        "mov %0, x1\n"
        "mov %1, x2\n"
        "mov %2, x3\n"
        "mov %3, x4\n"
        "mov %4, x5\n"
        "mov %5, x6\n"
        "mov %6, x7\n"
        : "=r"(args[0]), "=r"(args[1]), "=r"(args[2]), "=r"(args[3]),
          "=r"(args[4]), "=r"(args[5]), "=r"(args[6])
        :
        : "x1", "x2", "x3", "x4", "x5", "x6", "x7", "memory");
}

static void vprintf_core(putchar_func_t putchar, sendstr_func_t sendstr,
                         const char *fmt, uint64_t *args) {
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
                    sendstr(buf);
                    break;

                case 'u':
                    if (is_long)
                        utoa_dec64((uint64_t)args[arg_idx++], buf);
                    else
                        utoa_dec((uint32_t)args[arg_idx++], buf);
                    sendstr(buf);
                    break;

                case 'x':
                    if (is_long)
                        utoa_hex64((uint64_t)args[arg_idx++], buf);
                    else
                        utoa_hex((uint32_t)args[arg_idx++], buf);
                    sendstr(buf);
                    break;

                case 'c':
                    putchar((char)args[arg_idx++]);
                    break;

                case 's':
                    sendstr((char *)args[arg_idx++]);
                    break;

                case '%':
                    putchar('%');
                    break;

                default:
                    putchar('%');
                    if (is_long) putchar('l');
                    putchar(*fmt);
                    break;
            }
        } else {
            putchar(*fmt);
        }
        fmt++;
    }
}

/* Async getchar */
char getchar() {
    char c_buf[1];
    while (1) {
        int len = mini_uart_async_read(c_buf, 1);
        if (len == 0) {
            continue;
        } else {
            break;
        }
    }
    return c_buf[0];
}

/* Sync getchar */
char getchar_sync() { return mini_uart_sync_read(); }

/* Async printf. This function provides simple output through the mini
 * UART. Ensure that the number of arguments does not exceed 7.*/
void printf(const char *fmt, ...) {
    uint64_t args[7];
    save_args(args);
    vprintf_core(mini_uart_async_write, mini_uart_async_write_str, fmt, args);
}

/* Sync printf. This function provides simple output through the mini
 * UART. Ensure that the number of arguments does not exceed 7 */
void printf_sync(const char *fmt, ...) {
    uint64_t args[7];
    save_args(args);
    vprintf_core(mini_uart_sync_write, mini_uart_sync_write_str, fmt, args);
}

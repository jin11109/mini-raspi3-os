#include "utils.h"

#include "kernel/taskq.h"

#include "drivers/mini_uart.h"

#include "arg.h"
#include "def.h"
#include "string.h"

typedef void (*putchar_func_t)(const char);
typedef void (*sendstr_func_t)(const char*);

extern void console_putchar_default(const char c);
extern void console_putchar_sync(const char c);
extern void console_sendstr_default(const char* c);
extern void console_sendstr_sync(const char* c);
extern char console_getchar_default(void);
extern char console_getchar_sync(void);

static void vprintf_core(putchar_func_t putchar, sendstr_func_t sendstr,
                         const char* fmt, va_list args) {
    // int arg_idx = 0;
    char buf[64];

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            int is_long = 0;

            if (*fmt == 'l') {
                is_long = 1;
                fmt++;
            }

            switch (*fmt) {
            case 'd':
                if (is_long)
                    itoa_dec64(va_arg(args, int64_t), buf);
                else
                    itoa_dec(va_arg(args, int32_t), buf);
                /* TODO: Use put char only */
                sendstr(buf);
                break;

            case 'u':
                if (is_long)
                    utoa_dec64(va_arg(args, uint64_t), buf);
                else
                    utoa_dec(va_arg(args, uint32_t), buf);
                sendstr(buf);
                break;

            case 'x':
                if (is_long)
                    utoa_hex64(va_arg(args, uint64_t), buf);
                else
                    utoa_hex(va_arg(args, uint32_t), buf);
                sendstr(buf);
                break;

            case 'c':
                putchar((char)va_arg(args, int32_t));
                break;

            case 's':
                sendstr((char*)va_arg(args, char*));
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

/* Use async function as default */
char getchar(void) { return console_getchar_default(); }

/* Sync getchar */
char getchar_sync(void) { return console_getchar_sync(); }

/* Use async function as default */
void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf_core(console_putchar_default, console_sendstr_default, fmt, args);
    va_end(args);
}

/* Sync printf */
void printf_sync(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf_core(console_putchar_sync, console_sendstr_sync, fmt, args);
    va_end(args);
}

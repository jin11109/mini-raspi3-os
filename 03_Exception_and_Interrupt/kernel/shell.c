#include "command_registry.h"
#include "cpio.h"
#include "mbox.h"
#include "mini_uart.h"
#include "power.h"
#include "string.h"
#include "utils.h"

#define MAX_BUF 2048

/* Read and print the input of shell, and return input size */
size_t read_line(char *buf) {
    int pos = 0;
    while (1) {
        char c = getchar();
        if (c == '\r' || c == '\n') {
            printf("\r");
            printf("\n");
            buf[pos] = '\0';
            
            return pos;
        } else if (c == 0x08 || c == 0x7F) {
            if (pos > 0) {
                pos--;
                printf("\b");
                printf(" ");
                printf("\b");
            }
        } else if (pos < MAX_BUF - 2) {
            buf[pos++] = c;
            printf("%c", c);
        } else {
            printf("Out of shell buffer\r\n");
        }
    }
}

void shell() {
    char buf[MAX_BUF];
    while (1) {
        printf("\x1b[1;34mmini-raspi3-os$ \x1b[0m");

        size_t buf_len = read_line(buf);
        int argc = (int)count_substr(buf, ' ', buf_len);
        char* argv[argc];
        argc = split_inplace(argv, buf, ' ', argc);

        if (argc == 0) continue;

        command_fn_t fn = find_command(argv[0]);
        if (fn) {
            fn(argc, argv);
        } else {
            printf("command not found\r\n");
        }
    }
}
#include "shell.h"

#include "cpio.h"
#include "mbox.h"
#include "mini_uart.h"
#include "power.h"
#include "string.h"
#include "utils.h"

void read_line(char *buf) {
    int pos = 0;
    while (1) {
        char c = getchar();
        if (c == '\r' || c == '\n') {
            printf("\r");
            printf("\n");
            buf[pos] = '\0';
            break;
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

void execute_cmd(char *buf) {
    if (strcmp(buf, "help") == 0) {
        printf(
            "help    : Print this help menu.\r\n"
            "hello   : Print Hello World!\r\n"
            "reboot  : Reboot afetr 16 ticks.\r\n"
            "cancel_reboot : Before watchdog time expire you canel "
            "reboot.\r\n"
            "lshw    : List hardware information\r\n");
    } else if (strcmp(buf, "hello") == 0) {
        printf("Hello World!\r\n");
    } else if (strcmp(buf, "reboot") == 0) {
        uart_flush_recv();
        reboot(16);
    } else if (strcmp(buf, "cancel_reboot") == 0) {
        cancel_reboot();
    } else if (strcmp(buf, "lshw") == 0) {
        get_hardware_info();
    } else if (strcmp(buf, "ls") == 0) {
        parse_cpio((char *)(0x20000000));
    }
}

void shell() {
    char buf[MAX_BUF];
    while (1) {
        printf("\x1b[1;34mmini-raspi3-os$ \x1b[0m");

        read_line(buf);
        execute_cmd(buf);
    }
}
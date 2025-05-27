#include <mini_uart.h>
#include <shell.h>

void read_line(char *buf) {
    int pos = 0;
    while (1) {
        char c = uart_recv();
        if (c == '\r' || c == '\n') {
            uart_send('\r');
            uart_send('\n');
            buf[pos] = '\0';
            break;
        } else if (c == 0x08 || c == 0x7F) {
            if (pos > 0) {
                pos--;
                uart_send('\b');
                uart_send(' ');
                uart_send('\b');
            }
        } else if (pos < MAX_BUF - 2) {
            buf[pos++] = c;
            uart_send(c);
        } else {
            uart_send_string("Out of shell buffer\r\n");
        }
    }
}

void execute_cmd(char *buf) {
    if (buf[0] == 'h') {
        uart_send_string("h : print this help menu\r\n");
    }
}

void shell() {
    char buf[MAX_BUF];
    while (1) {
        uart_send_string("\x1b[1;34mmini-raspi3-os$ \x1b[0m");
        
        read_line(buf);
        execute_cmd(buf);
    }
}
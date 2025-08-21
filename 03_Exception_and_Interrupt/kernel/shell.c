#include "command_registry.h"
#include "cpio.h"
#include "malloc.h"
#include "mbox.h"
#include "mini_uart.h"
#include "power.h"
#include "string.h"
#include "utils.h"

#define MAX_BUF 2048

#ifdef DEBUG
void test_user_prog() {
    __asm__ volatile(
        "mov x0, 0\n"
        "loop:\n"
        "add x0, x0, 1\n"
        "svc 0\n"
        "cmp x0, 5\n"
        "blt loop\n"
        "b .\n"
        :
        :
        : "x0", "memory");
}
#endif

void execute_user_prog(void* data_ptr) {
    // char* user_stack_top = ((char*)malloc(4 * 1024)) + 4 * 1024;
    char user_stack_top[8 * 1024];
    __asm__ volatile(
        "msr sp_el0, %0\n"
        "mov x0, #0\n"
        "msr spsr_el1, x0\n"
        "msr elr_el1, %1\n"
        "eret\n"
        :
        : "r"(&user_stack_top[8 * 1024]), "r"(data_ptr)
        : "x0", "memory");
}

/* Read and print the input of shell, and return input size */
size_t read_line(char* buf) {
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

        // Search buildin command
        command_fn_t fn = find_command(argv[0]);
        if (fn) {
            fn(argc, argv);
            continue;
        }
        // Search user programs
        void* data_ptr = cpio_get_executable_file(argv[0]);
        if (data_ptr) {
            execute_user_prog(data_ptr);
            continue;
        }

        printf("command not found\r\n");
    }
}
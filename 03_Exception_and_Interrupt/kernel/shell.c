#include "command_registry.h"
#include "cpio.h"
#include "malloc.h"
#include "mbox.h"
#include "mini_uart.h"
#include "mm.h"
#include "power.h"
#include "string.h"
#include "utils.h"

#define MAX_BUF 2048
#define HISTORY_SIZE 16
#define PROMPT "\x1b[1;34mmini-raspi3-os$ \x1b[0m"

char history[HISTORY_SIZE][MAX_BUF];
char buf[MAX_BUF];
int history_count = 0;
int history_pos = 0;

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

inline static void clear_line(size_t n) {
    n += sizeof(PROMPT) - 1;
    char clear[n + 1];
    clear[n] = '\0';
    memset(clear, ' ', n);
    printf("\r%s", clear);
}

/* Read and print the input of shell, and return input size */
size_t read_line(char* buf) {
    int pos = 0;
    int esc_state = 0; // 0=normal, 1=ESC, 2=ESC[

    while (1) {
        char c = getchar();

        if (esc_state == 0) {
            if (c == '\x1b') { // esc
                esc_state = 1;
                continue;
            }
        } else if (esc_state == 1) {
            if (c == '[') {
                esc_state = 2;
                continue;
            } else {
                esc_state = 0;
            }
        } else if (esc_state == 2) {
            if (c == 'A') { // up button
                clear_line(pos + 1);

                if (history_pos > 0) history_pos--;
                memcpy(buf, history[history_pos],
                       strlen(history[history_pos]) + 1);
                // Point to '\0'
                pos = strlen(buf);

                printf("\r" PROMPT "%s", buf);
            } else if (c == 'B') { // down button
                clear_line(pos + 1);

                if (history_pos < history_count) history_pos++;
                if (history_pos == history_count) {
                    buf[0] = '\0';
                    pos = 0;
                } else {
                    memcpy(buf, history[history_pos],
                           strlen(history[history_pos]) + 1);
                    // Point to '\0'
                    pos = strlen(buf);
                }

                printf("\r" PROMPT "%s", buf);
            }
            esc_state = 0;
            continue;
        }

        if (c == '\r' || c == '\n') {
            printf("\r\n");
            buf[pos] = '\0';

            if (pos > 0) {
                // History buffer include '\0'
                memcpy(history[history_count % HISTORY_SIZE], buf, pos + 1);
                history_count++;
            }
            history_pos = history_count;

            return pos;
        } else if (c == 0x08 || c == 0x7F) {
            if (pos > 0) {
                pos--;
                printf("\b \b");
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
#ifdef TEST_INTERRUPT
    /* At the begining of shell, set cpu timmer interrupt (high priority) */
    command_fn_t fn = find_command("settimeout");
    char* a[] = {"settimeout", "1", "1"};
    if (fn) {
        fn(3, a);
    }

    /* mini uart TX interrupt (low priority) */
    printf("%s", "test interrupt\r\n");
#endif

    while (1) {
        printf(PROMPT);

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
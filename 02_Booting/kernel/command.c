#include "command_registry.h"
#include "utils.h"

void cmd_echo(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        printf(argv[i]);
        printf(" ");
    }
    printf("\r\n");
}
COMMAND_DEFINE("echo", cmd_echo);

void cmd_help(int argc, char **argv) {
    printf("help    : Print this help menu.\r\n");
    list_commands();
}
COMMAND_DEFINE("help", cmd_help);

void cmd_hello(int argc, char **argv) { printf("Hello World!!\r\n"); }
COMMAND_DEFINE("hello", cmd_hello);
#include "command_registry.h"

#include "string.h"
#include "utils.h"

extern command_entry_t __command_start[];
extern command_entry_t __command_end[];

command_fn_t find_command(const char *name) {
    for (command_entry_t *p = __command_start; p < __command_end; p++) {
        if (strcmp(name, p->name) == 0) return p->fn;
    }
    return 0;
}

void list_commands(void) {
    for (command_entry_t *p = __command_start; p < __command_end; p++) {
        printf("%s\r\n", p->name);
    }
}

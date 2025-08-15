#ifndef _COMMAND_REGISTRY_H
#define _COMMAND_REGISTRY_H

typedef void (*command_fn_t)(int argc, char **argv);

typedef struct {
        const char *name;
        command_fn_t fn;
} command_entry_t;

extern command_entry_t __command_start[];
extern command_entry_t __command_end[];

command_fn_t find_command(const char *name);
void list_commands(void);

#define COMMAND_DEFINE(name_str, func)        \
    static const command_entry_t __cmd_##func \
        __attribute__((section(".command_table"), used)) = {name_str, func}

#endif /* _COMMAND_REGISTRY_H */

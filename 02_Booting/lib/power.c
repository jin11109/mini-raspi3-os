#include "peripherals/power.h"

#include "command_registry.h"
#include "power.h"
#include "utils.h"

void reboot(int tick) {
    put32(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick
    put32(PM_RSTC, PM_PASSWORD | 0x20); // full reset
}

void cancel_reboot() {
    put32(PM_RSTC, PM_PASSWORD | 0);
    put32(PM_WDOG, PM_PASSWORD | 0);
}

void cmd_reboot(int argc, char** argv) { reboot(16); }
COMMAND_DEFINE("reboot", cmd_reboot);

void cmd_cancel_reboot(int argc, char** argv) { cancel_reboot(); }
COMMAND_DEFINE("cancel_reboot", cmd_cancel_reboot);
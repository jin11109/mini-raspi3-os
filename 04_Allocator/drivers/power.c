#include "drivers/power.h"

#include "peripherals/power.h"

#include "command_registry.h"
#include "utils.h"

void reboot(int tick) {
    MMIO_WRITE32(PM_PASSWORD | tick, PM_WDOG);  // number of watchdog tick
    MMIO_WRITE32(PM_PASSWORD | 0x20, PM_RSTC);  // full reset
}

void cancel_reboot() {
    MMIO_WRITE32(PM_PASSWORD | 0, PM_RSTC);
    MMIO_WRITE32(PM_PASSWORD | 0, PM_WDOG);
}

void cmd_reboot(int argc, char** argv) { reboot(16); }
COMMAND_DEFINE("reboot", cmd_reboot);

void cmd_cancel_reboot(int argc, char** argv) { cancel_reboot(); }
COMMAND_DEFINE("cancel_reboot", cmd_cancel_reboot);

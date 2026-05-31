#include "power.h"

#include "peripherals/power.h"

#include "utils.h"

void cancel_reboot() {
    MMIO_WRITE32(PM_PASSWORD | 0, PM_RSTC);
    MMIO_WRITE32(PM_PASSWORD | 0, PM_WDOG);
}

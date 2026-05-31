#ifndef _INCLUDE_DRIVERS_POWER_H
#define _INCLUDE_DRIVERS_POWER_H

/* Reboot after watchdog timer expire */
void reboot(int tick);
void cancel_reboot();

#endif /* _INCLUDE_DRIVERS_POWER_H */

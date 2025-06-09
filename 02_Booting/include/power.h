#ifndef _POWER_H
#define _POWER_H

/* Reboot after watchdog timer expire */
void reboot(int tick);
void cancel_reboot();

#endif /*_POWER_H */
# Implemented Features

1. Set up **mini UART** as a bridge between rpi3 and host computer
2. **Simple shell** with following commands:
    ```
    help    : Print this help menu.
    hello   : Print Hello World!
    reboot  : Reboot afetr 16 ticks.
    cancel_reboot : Before watchdog time expire you canel reboot.
    lshw    : List hardware information
    ```
    - `reboot`: Uses **power management registers**.
    - `lshw`: Gets hardware information via **mailbox**.

## demo (in Raspi3b+):
- `help`:
```
mini-raspi3-os$ help
help    : Print this help menu.
hello   : Print Hello World!
reboot  : Reboot afetr 16 ticks.
cancel_reboot : Before watchdog time expire you canel reboot.
lshw    : List hardware information
```
- `hello`:
```
mini-raspi3-os$ hello
Hello World!
```
- `lshw`:
```
mini-raspi3-os$ lshw
Firmware Version: 0x68248b9e
Board Revision: 0xa020d4
MAC Address: b8:27:eb:eb:27:b8
ARM Memory: base=0x80000008, size=0x0
VC Memory : base=0x8, size=0x80000008
CPU Clock : 4 Hz
CPU Temp  : 0.0 °C
```

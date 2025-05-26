# About this repository
This repository was created step by step by following **Professor Tsung Tai Yeh**’s [OSC course](https://people.cs.nycu.edu.tw/~ttyeh/course/2024_Spring/IOC5226/outline.html) ([labs](https://nycu-caslab.github.io/OSC2024/)) at NYCU. Its goal is to **build a simple operating system** on the **Raspberry Pi 3B+**. 

Special thanks to the professor for making these learning resources publicly available.

# Quick Start
1. Enter any lesson directory and set up the environment:
    ```bash
    # For example, in 01_HelloWorld/
    $ . ../env.sh
    ```
2. Use the Makefile to build `kernel8.img`:
    ```bash
    $ make
    ```
    Or build `kernel8.img` and run it in QEMU:
    ```bash
    $ make qemu
    ```
## Debug way
After entering a lesson directory and setting up the environment, use the following command to start debugging with QEMU.  
**Warning**: This command will first remove `./build/*` and `kernel8.img`.
```bash
$ make qemu-debug
```
Then, in another terminal, run:
```
$ aarch64-none-elf-gdb ./build/kernel8.elf
$ (gdb) target remote :1234
```

# Development Notes
- [Lab 0: Environment Setup](https://hackmd.io/@0a2xfYPxSzGdcXv06JB8_Q/rkHdpYkZxe)

- [Lab 1: Hello World](https://hackmd.io/@0a2xfYPxSzGdcXv06JB8_Q/SJP2qXEbeg)

# References
- [mini-arm-os](https://github.com/jserv/mini-arm-os/tree/master)
- [raspi3-tutorial](https://github.com/bztsrc/raspi3-tutorial/tree/master)
- [raspberry-pi-os](https://github.com/s-matyukevich/raspberry-pi-os)
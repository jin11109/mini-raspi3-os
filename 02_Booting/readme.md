# Quick start
## Run on Raspberry Pi 3B+
1. Download the following files and place them in the SD card's boot partition:
    - [bcm2710-rpi-3-b-plus.dtb](https://github.com/raspberrypi/firmware/raw/master/boot/bcm2710-rpi-3-b-plus.dtb)
    - [fixup.dat](https://github.com/raspberrypi/firmware/blob/master/boot/fixup.dat)
    - [start.elf](https://github.com/raspberrypi/firmware/blob/master/boot/start.elf)
    - [bootcode.bin](https://github.com/raspberrypi/firmware/blob/master/boot/bootcode.bin)
2. Build the kernel+bootloader image(`kernel8.img`) using `make`:
    ```bash
    $ make
    ```
3. Copy the following files to the SD card:
    - `kernel8.img`
    - `config.txt`
    - `initramfs.cpio`

4. Insert the SD card into the Raspberry Pi 3B+ and power it on. Then connect to the serial console using:
    ```bash
    sudo screen /dev/<device_path> 115200
    ```

### UART-based bootloader
To update the kernel without physically accessing the SD card (i.e., over mini-UART), follow the steps below. This method also works with QEMU.

1. Exit the current `screen` session:
    ```screen
    // Inside the screen session:
    Press Ctrl+A, then press K, and confirm with 'y' to kill the session.
    ```
2. After preparing kernel image(`kernel8_pure.img`) by `make kernel-pure`, use the provided Python script to send it via mini-UART:
    ```bash
    # From the ./host directory
    $ sudo -E PYTHONPATH=$PYTHONPATH python3 send_img.py
    ```

## Run on QEMU
1. Run the provided QEMU startup script:
    ```bash
    ./run_qemu.sh
    ```
    For debugging mode, use:
    ```bash
    ./run_qemu.sh -debug
    ```
2. If running in debug mode, open another terminal and start GDB:
    ```bash
    $ aarch64-none-elf-gdb ./build/kernel8.elf
    (gdb) target remote :1234
    ```

# Implemented Features

1. **UART Bootloader for Remote Kernel Loading**  
Implemented a UART-based bootloader that supports remote kernel loading via serial communication(Mini-uart). This enables kernel image transmission without physically accessing the SD card.

2. **Initial Ramdisk (initramfs) Support**  
Integrated support for loading and parsing `.cpio` archive files as an initial ramdisk (`initramfs.cpio`). Built-in shell commands such as `ls` and `cat` are provided to browse and inspect the contents of the ramdisk filesystem in memory.

3. **Heap Memory Allocator (malloc)**  
Developed a simple heap-based memory allocator (`malloc`) to support dynamic memory allocation for the kernel and bootloader.

4. **Bootloader Self-Relocation Mechanism**  
Added a self-relocation feature to the bootloader. When loading a kernel, the bootloader relocates itself to a reserved memory region, allowing the kernel to be loaded at the default execution address (`0x80000`) without requiring kernel-side address layout modifications.

5. **Flattened Device Tree (FDT) Parsing**  
Implemented a basic FDT (Flattened Device Tree) parser that enables the kernel to automatically retrieve hardware-related information, including the address of the initial ramdisk, by traversing the device tree provided by the bootloader and firmware.

6. **Modular Command Registration System**  
Introduced a modular command registration mechanism that allows shell commands to be automatically registered at compile-time using linker sections. This design decouples command implementation from the shell and enables easy extensibility for future commands.

# DEMO
## Run on Raspberry Pi 3B+
- **Start Raspberry Pi 3B+**
    ```bash
    Bootloader Start
    [1] Load Kernel from mini UART
    [2] Boot from existing Kernel
    Enter choice: 
    ```
- **UART Bootloader**
    ```
    Bootloader Start
    [1] Load Kernel from mini UART
    [2] Boot from existing Kernel
    Enter choice: 
    ```
    - Press `1` for loading kernel from mini UART.
        ```bash
        Enter choice: 1
        Loading kernel from mini UART...
        ```
        After quit `screen` and prepare `kernel8_pure.img`
        ```bash
        // From the ./host directory
        $ python3 send_img.py 
        Original size: 1048576 bytes
        Compressed size: 27357 bytes
        CRC32: 0x6AB576F2
        Python-dotenv could not parse statement starting at line 1
        Input serial port path or press ENTER to use default path: 
        default path: /dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A50285BI-if00-port0
        Sending: 100%|██████████████████████████| 27369/27369 [00:02<00:00, 11619.58B/s]
        Waiting for response...
        CRC verified by client: OK
        Client responses message: b'Kernel received, received 27357 bytes, decompressed size 1048576\r\n'
        Send img done
        ```
    - Press `2` boot from existing kernel.
        ```bash
        Enter choice: 2
        Booting existing kernel...
        mini-raspi3-os$ 
        ```
- **Ramdisk and Flattened Device Tree (FDT) Parsing**
    - `ls`
        ```bash
        mini-raspi3-os$ ls
        .
        hellow.txt
        test.txt
        ```
    - `cat`
        ```bash
        mini-raspi3-os$ cat hellow.txt
        This is a testing file, named "hellow.txt".
        ```


## Run on QEMU
- **Start QEMU**
    ```bash
    $ ./run_qemu.sh 
    Build successfully: /dev/pts/10 <--> /dev/pts/11
    QEMU will use /dev/pts/10
    You can first use below command to interact with QEMU
        $ screen /dev/pts/11 115200
    Press enter and continue QUMU

    QEMU 6.2.0 monitor - type 'help' for more information
    (qemu) 
    ```
    Another terminal `screen`:
    ```bash
    Bootloader Start
    [1] Load Kernel from mini UART
    [2] Boot from existing Kernel
    Enter choice: 
    ```
- **UART Bootloader**
    ```
    Bootloader Start
    [1] Load Kernel from mini UART
    [2] Boot from existing Kernel
    Enter choice: 
    ```
    - Press `1` for loading kernel from mini UART.
        ```bash
        Enter choice: 1
        Loading kernel from mini UART...
        ```
        After quit `screen` and prepare `kernel8_pure.img`
        ```bash
        // From the ./host directory
        $ sudo -E PYTHONPATH=$PYTHONPATH python3 send_img.py
        Original size: 1048576 bytes
        Compressed size: 27357 bytes
        CRC32: 0x6AB576F2
        Python-dotenv could not parse statement starting at line 1
        Input serial port path or press ENTER to use default path: /dev/pts/11
        Sending: 100%|█████████████████████████| 27369/27369 [00:00<00:00, 156266.38B/s]
        Waiting for response...
        CRC verified by client: OK
        Client responses message: b'Kernel received, received 27357 bytes, decompressed size 1048576\r\n'
        Send img done
        ```
    - Press `2` boot from existing kernel.
        ```bash
        Enter choice: 2
        Booting existing kernel...
        mini-raspi3-os$ 
        ```
- **Ramdisk and Flattened Device Tree (FDT) Parsing**
    - `ls`
        ```bash
        mini-raspi3-os$ ls
        .
        hellow.txt
        test.txt
        ```
    - `cat`
        ```bash
        mini-raspi3-os$ cat hellow.txt
        This is a testing file, named "hellow.txt".
        ```
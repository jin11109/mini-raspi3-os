# Quick start
## Run on Raspberry Pi 3B+
1. Download the following files and place them in the SD card's boot partition:
    - [bcm2710-rpi-3-b-plus.dtb](https://github.com/raspberrypi/firmware/raw/master/boot/bcm2710-rpi-3-b-plus.dtb)
    - [fixup.dat](https://github.com/raspberrypi/firmware/blob/master/boot/fixup.dat)
    - [start.elf](https://github.com/raspberrypi/firmware/blob/master/boot/start.elf)
    - [bootcode.bin](https://github.com/raspberrypi/firmware/blob/master/boot/bootcode.bin)
2. Build the kernel+bootloader image(`kernel8.img`) using `make`:
    ```console
    $ make
    ```
    Build image with makefile falgs:
    - For running on raspi, use `RASPI=1`. It will use different constant number.
        ```console
        $ make RASPI=1
        ```
    - For debug, use `ENABLE_DEBUG=1`. It will add `-g` to gcc compile flags, print some basic information and use strong memory barrier for `MMIO` operations (e.g. `MMIO_WRITE32()`, `MMIO_READ32()`).
        ```console
        $ make ENABLE_DEBUG=1
        ```
    - For testing interrupt, use `ENABLE_TEST_INTERRUPT=1`. It will add time consuming process in some bottom-half interrupt task. It also let **cpu timer interrup**t and **mini uart tx interrupt** occur automatically before starting shell. For detail, see [Interrupt System](#1-interrupt-system) and [demo: test interrupt](#test-interrupt).
        ```console
        $ make ENABLE_TEST_INTERRUPT=1
        ``` 

3. Copy the following files to the SD card:
    - `kernel8.img`
    - `config.txt`
    - `initramfs.cpio`

4. Insert the SD card into the Raspberry Pi 3B+ and power it on. Then connect to the serial console using:
    ``` console
    $ sudo screen /dev/<device_path> 115200
    ```

### UART-based bootloader
See [UART-based bootloader](/02_Booting/readme.md#uart-based-bootloader) in lab2.

## Run on QEMU
See [Run on QEMU](/02_Booting/readme.md#run-on-qemu) in lab2.

# Implemented Features

## Exception Level Switch

### EL2 to EL1
On power-up, the Raspberry Pi starts in EL2. It switches to EL1 at `kernel_entry.S`, where the shell loop runs in EL1 mode.

### EL1 to EL0 and EL0 to EL1
EL1 switches to EL0 to execute user programs. When a user program issues an `SVC` (system call), execution switches back to EL1 to handle the syscall, then returns to EL0.

#### Support for exit syscall
When a user program issues `SVC 93`, it exits the program and control returns to the shell loop in EL1.
```
| +----------------------------------------+
| | shell() main loop in shell.c           |
| +----------------------------------------+
|        |
|        | If the command is not buildin command
|        V
|    +------------------------------------+
E    | cpio_get_executable_file in cpio.c |
L    +------------------------------------+
1        |
|        | If found excuable file
|        V   
|    +---------------------------------+
|    | execute_user_prog() in shell.c  |
|    +---------------------------------+
|    1. Create user stack (assign EL0 stack)
|    2. Swich EL1 stack to EL0
|    3. Swich EL1 to EL0
|        3.1 Set spsr_el1 register to zero
|            3.1.1 Enable all ineterrupt
|            3.1.2 EL0t mode
|            3.1.3 Set all condition flags to zero
|        3.2 Set elr_el1 to begining of user program  
        |
        V  
| +---------------------------+
| | Begining of user program  |
E +---------------------------+
L   .
0   . (some instruciotns)
|   .
|   SVC #n
    |    1. Switch to EL1 by hardware
    |        |      
    |        V         
    |    +------------------------------------+
    |    | exception_vector_table in vecotr.S |
    |    +------------------------------------+
    E        |      
    L        V 
    1    +-----------------------------+
    |    | sync_handler in exception.S |
    |    +-----------------------------+
    |    1. Disable all ineterrupt
    |    2. Save user context
    |        |                     |
    |        | Other SVC           | if SVC #93
    |        V                     |
    |    1. print "Unknown SVC #n" |
    |       on screen              V     
    |    +------------------+    +----------------+
    |    | sync_handler_end |    | sys_exit       | 
    |    | in exception.S   |    | in exception.S |
    |    +------------------+    +----------------+
    |    1. Restore user         1. Enable ineterrupt  
    |       context              2. jump to shell      
    |    2. Enable ineterrupt            |                  
    |    3. jump back to user program    |                 
    |       3.1 switch EL1 to EL0        |                       
    |         |                          |
|   .  <------+                          |
E   . (some instruciotns)                |
L   .                                    |
0 +----------------------+               |
| | End of user program  |               |
| +----------------------+               |
                                         |
                                         V
| +-----------------------------------------+ 
E | shell() main loop in shell.c            | 
L +-----------------------------------------+ 
1
|
```


## Core Timer interrupt Timer Multiplexing

## Interrupt System
### Support
- preeempt
- top/bottom half
- priority
    - high: cpu timer
    - normal: mini uart rx
    - low: mini uart tx
```
+-------------------+
| Interrupt occur   |
+-------------------+
    |
    V
+------------------------------------------------------------+
| exception_vector_table in vector.S                         |
+------------------------------------------------------------+
Only IRQ interrupt have supported, other interrupt, such as FIQ,
SError and Synchronous will jump to exception_handler
    |                                      |
    | IRQ interrupt (EL1h, EL064)          | Other Inetrrupt
    V                                      V
+--------------------------+  +----------------------------------+
| irq_entry in exception.S |  | exception_handler in exception.S |
+--------------------------+  +----------------------------------+
1. Disable all interrupt      1. Print register value (disable now) 
2. Save all context           2. ret
3. Jump to irq_handler
|        |
|        V
|    +-----------------------------------------------------+
|    | irq_handler() in irq.c                              |
|    +-----------------------------------------------------+
|    For loop to test the interrupt is local irq or SoC irq
|    and call then one by one
C        |                             +-------+
P        | local irq (only core0)      |       | Soc irq
U        V                             |       V
     +-------------------------------+ |  +----------------------+    
I    | local_irq_table[0][i].handler | |  | irq_table[i].handler |
N    +-------------------------------+ |  +----------------------+ 
T        |                             |      |                
E        | Core0 timer int             |      | AUX int
R        V                             |      V              
R    +---------------------------+     |  +-------------------+ 
U    | timer_interrupt_handler() |     |  | aux_irq_handler() |
P    | in mini_uart.c            |     |  | in aux.c          |
T    +---------------------------+     |  +-------------------+
     1. Schedule task                  |      |     |
D    2. Return  ->---------------------+      |     |
I                  Return to irq_handler()    |     |
S                                             |     |          
A                            +----------------+     |
B                            | TX int               | RX int
L                            V                      V
E                    +-----------------+  +-------------------+  
|                    | mini_uart_tx_top|  | mini_uart_rx_top  | 
|                    | in mini_uart.c  |  | in mini_uart.c    | 
|                    +-----------------+  +-------------------+  
|                    1. Mask tx int       1. Mask rx int    
|                    2. Schedule task     2. Read form uart  
|                    3. Return            If ENABLE_TEST_INTERRUPT 
|                        |                3. Schedule task
|                        |                else
|                        |                3. Unmask rx int 
|                        |                4. Return
|                        | irq_handler()     | irq_handler()
|                        | return            | return
|                        V                   V
+-------------------------------------------------------+
| irq_entry in exception.S                              |
+-------------------------------------------------------+ 
4. Enable all interrupt
5. Jump to process_task (bottom half)
|    |
|    V
|    +-------------------------------+                            ^
|    | process_task() in taskq.c     |    back to Interrupt Occur |
|    +-------------------------------+                            |
|                                                                 |
|    1. Process task ->--------Interrupt Occur again--------------+   
|    2. Unnask the device int      
|    3. Return
|    |
|    V
+-----------------------------------------------+
| irq_entry in exception.S                      |
+-----------------------------------------------+ 
6. Load all context
7. eret

```
## New console feature


# DEMO
## Execute user program
## Test interrupt system
If use 
```
Bootloader Start
[1] Load Kernel from mini UART
[2] Boot from existing Kernel
Enter choice: 2
Booting existing kernel...
[test interrupt] (prio:low) mini uart tx bottom start
t[test interrupt] Task preempt
[test interrupt] (prio:normal) mini uart rx bottom start
[test interrupt] Task preempt
[timeout] (prio:high) 1
[test interrupt] Nest interrupt 2
[test interrupt] (prio:normal) mini uart rx bottom end 321730048
[test interrupt] Nest interrupt 1
est interrupt
[test interrupt] (prio:low) mini uart tx bottom end 852713472
```

### dismantle
Press `2` on the kerboard to booting the existing kernel.
```
Bootloader Start
[1] Load Kernel from mini UART
[2] Boot from existing Kernel
Enter choice: 2
Booting existing kernel...
```
---------
Because of compiling the kernel by the flags `ENABLE_TEST_INTERRUPT=1`, we already add some events which can occur different interrupts brfore shell begin.
- We first set cpu timer scheduler by using `settimeout 1 1`. This will cause a cpu timer interrupt after 1 seconds. And print `[timeout] (prio:high) 1` at the screen. 
- Then, staring send `test interrupt` on the screen by mini uart. This will frist cause a TX interrupt and enqueue the sending task in taskq(with low priority). Specially, we intentionally add a time consuming process after sending a character. This aim to give a chance that cpu timer inetrrupt(or mini uart rx interrupt) with high priority can preempt this seanding task.  
Morover,   
    - when mini uart tx bottom task start, it print at the begining of this task  
    `[test interrupt] (prio:low) mini uart tx bottom start`   
    - At the end of task, it print  
    `[test interrupt] (prio:low) mini uart tx bottom end 852713472`(ignore the number).

    It can help us to observe that the task has been interrupt or not.
    > We do the same process in the mini uart rx
------
We can found that cpu get a TX interrupt and start to run sending task in the taskq by the first line of message on the screen. It is in the bottom half process in this moment, other ineterrupt can interrupt this task.
```
[test interrupt] (prio:low) mini uart tx bottom start
```
-----
In the sending task. we successfully send a character `t` on the screen, which is first letter of `test interrupt`. Then, starting Execute the time consuming process.  
While excuting the time consuming process, We artificially press `2` on the keyboard which cause **mini uart RX interrupt**. So, you can see the message `[test interrupt] Task preempt`, which means TX bottom-half task has been preempt, because RX have higher priority.
```
[test interrupt] (prio:low) mini uart tx bottom start
t[test interrupt] Task preempt
[test interrupt] (prio:normal) mini uart rx bottom start
```
---
In rapid sequence, cpu timer interrupt occur and preeempt the task again. So, you can see `[test interrupt] Task preempt` again, because timer task has higher priority than RX. Then, print timeout message `[timeout] (prio:high) 1`.  
After timer bottom-half has done. The end of function `process_task()`(in taskq.c) will count how many preempt had occured which means number of nesting interrupt, such as this case shows ther is two nest interrupt.
```
[test interrupt] Task preempt
[timeout] (prio:high) 1
[test interrupt] Nest interrupt 2
```
---
Return back to the RX bottom-half task, and wait to complete time consuming process.
Afetr RX bottom-hslf task has been done, run to count nesting interrupt again.
```
[test interrupt] (prio:normal) mini uart rx bottom end 321730048
[test interrupt] Nest interrupt 1
```
---
Finally, return bask to the TX bottom-half task, which want to finish sending `test interrupt` on the screen.
```
est interrupt
[test interrupt] (prio:low) mini uart tx bottom end 852713472
```
## Exception Level Switch and Execute User Program
```asm
.section ".text._start"
.global _start
_start:
    mov x0, 0
loop:
    add x0, x0, 1
    svc 0
    cmp x0, 5
    blt loop

    svc 93 // exit
```
```console
mini-raspi3-os$ build/test_exception.bin
[SVC] Unknown syscal: SVC #0
[SVC] Unknown syscal: SVC #0
[SVC] Unknown syscal: SVC #0
[SVC] Unknown syscal: SVC #0
[SVC] Unknown syscal: SVC #0
mini-raspi3-os$
```
## Run on Raspberry Pi 3B+


# TODO
- Add support for loading `.elf` user programs
    - Implement system calls
    - Provide wrappers for system calls (e.g., printf / printf_sync)
    - Test the interrupt system when user programs trigger interrupts via syscalls
- Eliminate malloc usage inside interrupt handlers
- Add support for reading board information
- Refactor the project structure, such as adding a dedicated driver directory
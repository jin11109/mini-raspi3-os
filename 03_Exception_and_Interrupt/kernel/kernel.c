#include "aux.h"
#include "cpio.h"
#include "def.h"
#include "fdt.h"
#include "fdt_callback.h"
#include "mini_uart.h"
#include "mm.h"
#include "shell.h"
#include "taskq.h"
#include "timer.h"
#include "utils.h"

#ifdef DEBUG
#define FDT_MAGIC 0xd00dfeed

void verify_dtb(uintptr_t dtb_addr) {
    uint32_t raw = *(volatile uint32_t*)dtb_addr;
    uint32_t magic = __builtin_bswap32(raw);

    if (magic == FDT_MAGIC) {
        printf_sync("Valid FDT at 0x%lx\r\n", dtb_addr);
    } else {
        printf_sync("Invalid DTB magic at 0x%lx\r\n", dtb_addr);
    }
}
#endif

void kernel_main(uint64_t dtb_addr, uint64_t x1, uint64_t x2) {
#ifdef DEBUG
    verify_dtb(dtb_addr);
#endif
    /* Init task queue */
    init_taskq();

    /* TODO: put all function of devices initialization together */
    /* Get intc device address */
    // fdt_traverse((void *)dtb_addr, intc_callback);
    /* Init mini uart */
    aux_init();
    mini_uart_async_init();
    /* Init timmer device */
    init_timer();
    /* Init cpio */
    fdt_traverse((void*)dtb_addr, initramfs_callback);
    init_cpio();

    shell();
}

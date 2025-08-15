#include "peripherals/mbox.h"

#include "command_registry.h"
#include "def.h"
#include "mbox.h"
#include "string.h"
#include "utils.h"

void get_hardware_info() {
    // Mailbox call will discard lower 4 bits.
    __attribute__((aligned(16))) unsigned int mailbox[40]; // buffer 大一點

    // Total buffer size in bytes.
    mailbox[0] = 40 * 4;
    // Request or response code.
    mailbox[1] = REQUEST_CODE;

    int i = 2;

    // Firmware version
    mailbox[i++] = GET_FIRMWARE_VERSION; // tag
    mailbox[i++] = 4;                    // value buffers size
    mailbox[i++] = TAG_REQUEST_CODE;     // tag request
    mailbox[i++] = 0;                    // values

    // Board revision
    mailbox[i++] = GET_BOARD_REVISION;
    mailbox[i++] = 4;
    mailbox[i++] = TAG_REQUEST_CODE;
    mailbox[i++] = 0;

    // MAC address
    mailbox[i++] = GET_BOARD_MAC_ADDR;
    mailbox[i++] = 6;
    mailbox[i++] = TAG_REQUEST_CODE;
    mailbox[i++] = 0;
    mailbox[i++] = 0;

    // ARM memory
    mailbox[i++] = GET_ARM_MEMORY;
    mailbox[i++] = 8;
    mailbox[i++] = TAG_REQUEST_CODE;
    mailbox[i++] = 0;
    mailbox[i++] = 0;

    // VC memory
    mailbox[i++] = GET_VC_MEMORY;
    mailbox[i++] = 8;
    mailbox[i++] = TAG_REQUEST_CODE;
    mailbox[i++] = 0;
    mailbox[i++] = 0;

    // CPU clock
    mailbox[i++] = GET_CLOCK_RATE;
    mailbox[i++] = 4;
    mailbox[i++] = TAG_REQUEST_CODE;
    mailbox[i++] = CLOCK_ID_ARM;

    // CPU 0 temperature
    mailbox[i++] = GET_TEMPERATURE;
    mailbox[i++] = 4;
    mailbox[i++] = TAG_REQUEST_CODE;
    mailbox[i++] = 0;

    // end
    mailbox[i++] = END_TAG;

    if (mailbox_call(mailbox, MAILBOX_PROPERTY_CHANNEL)) {
        printf("Firmware Version: 0x%x\r\n", mailbox[5]);
        printf("Board Revision: 0x%x\r\n", mailbox[9]);
        printf("MAC Address: %x:%x:%x:%x:%x:%x\r\n", (mailbox[13] >> 0) & 0xff,
               (mailbox[13] >> 8) & 0xff, (mailbox[13] >> 16) & 0xff,
               (mailbox[13] >> 24) & 0xff, (mailbox[14] >> 0) & 0xff,
               (mailbox[14] >> 8) & 0xff);

        printf("ARM Memory: base=0x%x, size=0x%x\r\n", mailbox[17],
               mailbox[18]);
        printf("VC Memory : base=0x%x, size=0x%x\r\n", mailbox[21],
               mailbox[22]);
        printf("CPU Clock : %u Hz\r\n", mailbox[26]);
        printf("CPU Temp  : %u.%u °C\r\n", mailbox[30] / 1000,
               (mailbox[30] % 1000) / 100); // millidegree
    } else {
        /* TODO : This error handling should be implemented. */
        printf("Mailbox call failed\r\n");
    }
}

int mailbox_call(unsigned int* mailbox, unsigned int channel) {
    // Combine the message address (upper 28 bits) with channel number (lower 4
    // bits)
    uint32_t addr = ((uint32_t)(uintptr_t)mailbox) & ~0xF;
    uint32_t message = addr | channel;

    // Wait until mailbox can write
    while (*MAILBOX_STATUS & MAILBOX_FULL);

    *MAILBOX_WRITE = message;

    while (1) {
        // Wait until mailbox can read
        while (*MAILBOX_STATUS & MAILBOX_EMPTY);

        uint32_t resp = *MAILBOX_READ;
        // Check if the value is the same as wrote
        if ((resp & 0xF) == channel && (resp & ~0xF) == addr) {
            return mailbox[1] == REQUEST_SUCCEED;
        }
    }
}

void cmd_lshw(int argc, char** argv) { get_hardware_info(); }
COMMAND_DEFINE("lshw", get_hardware_info);
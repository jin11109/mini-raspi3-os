#ifndef _P_MBOX_H
#define _P_MBOX_H

#include "def.h"
#include "peripherals/base.h"

#define MAILBOX_BASE (PBASE + 0x0000B880)

/* Mailbox registers */
#define MAILBOX_READ ((volatile uint32_t *)(MAILBOX_BASE + 0x00))
#define MAILBOX_STATUS ((volatile uint32_t *)(MAILBOX_BASE + 0x18))
#define MAILBOX_WRITE ((volatile uint32_t *)(MAILBOX_BASE + 0x20))

/* Flags */
#define MAILBOX_FULL 0x80000000
#define MAILBOX_EMPTY 0x40000000

/* Channel for power management */
#define MAILBOX_PM_CHANNEL 0
/* Channel for framebuffer */
#define MAILBOX_FRAMEBUFFER_CHANNEL 1
/* Channel for property tags ARM to VC */
#define MAILBOX_PROPERTY_CHANNEL 8

/* Mailbox tag values */
#define GET_FIRMWARE_VERSION 0x00000001
#define GET_BOARD_REVISION 0x00010002
#define GET_BOARD_MAC_ADDR 0x00010003
#define GET_ARM_MEMORY 0x00010005
#define GET_VC_MEMORY 0x00010006
#define GET_CLOCK_RATE 0x00030002
#define GET_TEMPERATURE 0x00030006

/* Request/response codes */
#define REQUEST_CODE 0x00000000
#define REQUEST_SUCCEED 0x80000000
#define REQUEST_FAILED 0x80000001
#define TAG_REQUEST_CODE 0x00000000
#define END_TAG 0x00000000

/* Clock IDs */
#define CLOCK_ID_EMMC 1
#define CLOCK_ID_UART 2
#define CLOCK_ID_ARM 3
#define CLOCK_ID_CORE 4
#define CLOCK_ID_V3D 5
#define CLOCK_ID_H264 6
#define CLOCK_ID_ISP 7
#define CLOCK_ID_SDRAM 8
#define CLOCK_ID_PIXEL 9
#define CLOCK_ID_PWM 10
#define CLOCK_ID_HEVC 11

int mailbox_call(unsigned int *mailbox, unsigned int channel);

#endif /*_P_MBOX_H */
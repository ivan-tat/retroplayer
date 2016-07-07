/* pic.c -- Intel 8259A Programmable Interrupt Controller interface.

   This is free and unencumbered software released into the public domain */

#ifdef __WATCOMC__
#include <i86.h>
#include <stdbool.h>
#include <stdint.h>
#include <conio.h>
#endif

#include "pic.h"

/* PIC1: IRQ 0-7 */
/* PIC2: IRQ 8-15 */

/* Output Control Word 1 (OCW1) */
/* Read/Write: Get/Set interrupt mask */
#define PIC1_IO_OCW1 0x21
#define PIC2_IO_OCW1 0xa1

/* Output Control Word 2 (OCW2) */
/* Write: Change IRQ priority; set EOI mode */
#define PIC1_IO_OCW2 0x20
#define PIC2_IO_OCW2 0xa0

/* OCW2 bits */
#define OCW2_COMMAND_MASK         0x18
/* always set this bits: */
#define OCW2_COMMAND_OCW2         0x00
/* user controlled bits: */
#define OCW2_LEVEL_MASK           0x07
#define OCW2_CMD_MASK             0xe0
#define OCW2_CMD_ROT_AUTO_EOI_CLR 0x00
#define OCW2_CMD_NONSPEC_EOI      0x20
#define OCW2_CMD_NOP              0x40
#define OCW2_CMD_SPEC_EOI         0x60
#define OCW2_CMD_ROT_AUTO_EOI_SET 0x80
#define OCW2_CMD_ROT_NONSPEC_EOI  0xa0
#define OCW2_CMD_SET_PRIORITY     0xc0
#define OCW2_CMD_ROT_SPEC_EOI     0xe0

/* Output Control Word 3 (OCW3) */
/* Write: Select Status Read mode */
#define PIC1_IO_OCW3 0x20
#define PIC2_IO_OCW3 0xa0

/* OCW3 bits */
#define OCW3_COMMAND_MASK 0x98
/* always set this bits: */
#define OCW3_COMMAND_OCW3 0x08
/* user controlled bits: */
#define OCW3_RMD_MASK      0x03
#define OCW3_RMD_NOP_0     0x00
#define OCW3_RMD_NOP_1     0x01
#define OCW3_RMD_READ_IR   0x02
#define OCW3_RMD_READ_IS   0x03
#define OCW3_POLL_MASK     0x04
#define OCW3_POLL_NONE     0x00
#define OCW3_POLL_CMD      0x04
#define OCW3_SPEC_MASK     0x60
#define OCW3_SPEC_NOP_0    0x00
#define OCW3_SPEC_NOP_1    0x20
#define OCW3_SPEC_RST_MASK 0x40
#define OCW3_SPEC_SET_MASK 0x60

void __far __pascal picEnableIRQs( uint16_t mask ) {
    if ( mask & 0xff ) {
        outp( PIC1_IO_OCW1, inp( PIC1_IO_OCW1 ) | ( uint8_t )( mask & 0xff ) );
    }
    if ( mask & 0xff00 ) {
        outp( PIC2_IO_OCW1, inp( PIC2_IO_OCW1 ) | ( uint8_t )( mask >> 8 ) );
    }
}

void __far __pascal picDisableIRQs( uint16_t mask ) {
    if ( mask & 0xff ) {
        outp( PIC1_IO_OCW1, inp( PIC1_IO_OCW1 ) & ( uint8_t )( ~( mask & 0xff ) ) );
    }
    if ( mask & 0xff00 ) {
        outp( PIC2_IO_OCW1, inp( PIC2_IO_OCW1 ) & ( uint8_t )( ~( mask >> 8 ) ) );
    }
}

void __far __pascal picEOI( uint8_t irq ) {
    outp( ( irq < 8 ) ? PIC1_IO_OCW2 : PIC2_IO_OCW2, OCW2_COMMAND_OCW2 | OCW2_CMD_NONSPEC_EOI );
}

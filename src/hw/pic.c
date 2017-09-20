/* pic.c -- Intel 8259A Programmable Interrupt Controller interface.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>
#include <conio.h>

// TODO: remove PUBLIC_CODE macros when done.

#include "pascal/pascal.h"
#include "cc/i86.h"
#include "ow/stdio.h"
#include "hw/pic.h"

/* PIC1: IRQ 0-7 */
/* This one is programmed by BIOS when PC is powered on */
static const uint8_t PIC1_VEC = 0x08;

/* PIC2: IRQ 8-15 */
/* This one is programmed by BIOS when PC is powered on */
static const uint8_t PIC2_VEC = 0x70;

#define picvec(ch) ((ch) < 8 ? PIC1_VEC + (ch) : PIC2_VEC + (ch) - 8)

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

void PUBLIC_CODE picEnableChannels(irqMask_t mask)
{
    if (mask & 0xff) {
        outp(PIC1_IO_OCW1, inp(PIC1_IO_OCW1) | (uint8_t)(mask & 0xff));
    }
    if (mask & 0xff00) {
        outp(PIC2_IO_OCW1, inp(PIC2_IO_OCW1) | (uint8_t)(mask >> 8));
    }
}

void PUBLIC_CODE picDisableChannels(irqMask_t mask)
{
    if (mask & 0xff) {
        outp(PIC1_IO_OCW1, inp(PIC1_IO_OCW1) & (uint8_t)(~(mask & 0xff)));
    }
    if (mask & 0xff00) {
        outp(PIC2_IO_OCW1, inp(PIC2_IO_OCW1) & (uint8_t)(~(mask >> 8)));
    }
}

void PUBLIC_CODE picEOI(uint8_t ch)
{
    if (ch >= 8) outp(PIC2_IO_OCW2, OCW2_COMMAND_OCW2 | OCW2_CMD_NONSPEC_EOI);
    outp(PIC1_IO_OCW2, OCW2_COMMAND_OCW2 | OCW2_CMD_NONSPEC_EOI);
}

void *PUBLIC_CODE picGetISR(uint8_t ch)
{
    void *p;
    getintvec(picvec(ch), &p);
    return p;
}

void PUBLIC_CODE picSetISR(uint8_t ch, void *p)
{
    setintvec(picvec(ch), p);
}

/* Sharing IRQ channels */

#include "isr.h"

typedef struct isrInfo_t {
    bool hooked;
    void *original;
    isrCallback_t *handler;
};

static struct isrInfo_t _isrList[IRQ_CHANNELS] = {
    { false, (void *)0, (void *)0 }
};

void __far __pascal _ISRCallback(uint8_t ch)
{
    isrCallback_t *handler;

    handler = _isrList[ch].handler;
    if (handler)
        handler(ch);
    else
        picEOI(ch);
}

void PUBLIC_CODE isrHookSingleChannel(uint8_t ch)
{
    if (! _isrList[ch].hooked)
    {
        _isrList[ch].original = picGetISR(ch);
        picSetISR(ch, getISR(ch));
        _isrList[ch].hooked = true;
    };
}

void PUBLIC_CODE isrReleaseSingleChannel(uint8_t ch)
{
    if (_isrList[ch].hooked) {
        picSetISR(ch, _isrList[ch].original);
        _isrList[ch].original = (void *)0;
        _isrList[ch].hooked = false;
    };
}

void PUBLIC_CODE isrSetSingleChannelHandler(uint8_t ch, isrCallback_t *p)
{
    _isrList[ch].handler = p;
}

isrCallback_t *PUBLIC_CODE isrGetSingleChannelHandler(uint8_t ch)
{
    return _isrList[ch].handler;
}

void PUBLIC_CODE isrClearSingleChannelHandler(uint8_t ch)
{
    _isrList[ch].handler = (void *)0;
}

/* Initialization */

void isrInit(void)
{
    int ch;
    for (ch = 0; ch < IRQ_CHANNELS; ch++) {
        _isrList[ch].hooked = false;
        _isrList[ch].original = (void *)0;
        _isrList[ch].handler = (void *)0;
    };
}

void isrDone(void)
{
    int ch;
    for (ch = 0; ch < IRQ_CHANNELS; ch++) {
        isrReleaseSingleChannel(ch);
        isrClearSingleChannelHandler(ch);
    };
}

DEFINE_REGISTRATION(pic, isrInit, isrDone)

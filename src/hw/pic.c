/* pic.c -- Intel 8259A Programmable Interrupt Controller interface.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/conio.h"
#include "cc/stdio.h"
#include "cc/stdlib.h"
#include "cc/string.h"
#include "debug.h"
#include "hw/hwowner.h"

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

/* ISR callback */

extern void *__far __cdecl isr_get(uint8_t ch);

/* Private PIC I/O */

void __near _picio_enable_irq_channels(IRQMASK mask)
{
    if (mask & 0xff)
        outp(PIC1_IO_OCW1, inp(PIC1_IO_OCW1) & (uint8_t)(~(mask & 0xff)));

    if (mask & 0xff00)
        outp(PIC2_IO_OCW1, inp(PIC2_IO_OCW1) & (uint8_t)(~(mask >> 8)));
}

void __near _picio_disable_irq_channels(IRQMASK mask)
{
    if (mask & 0xff)
        outp(PIC1_IO_OCW1, inp(PIC1_IO_OCW1) | (uint8_t)(mask & 0xff));

    if (mask & 0xff00)
        outp(PIC2_IO_OCW1, inp(PIC2_IO_OCW1) | (uint8_t)(mask >> 8));
}

void __near _picio_eoi1(uint8_t mode)
{
    outp(PIC1_IO_OCW2, OCW2_COMMAND_OCW2 | mode);
}

void __near _picio_eoi2(uint8_t mode)
{
    outp(PIC2_IO_OCW2, OCW2_COMMAND_OCW2 | mode);
}

/* Sharing IRQ channels */

static struct
{
    void *original;
    HWOWNER *owner;
    ISROWNERCALLBACK *handler;
    void *data;
} _isr_list[IRQ_CHANNELS] =
{
    { NULL, NULL, NULL, NULL }
};

static IRQMASK _isr_mask = 0;

#define _is_hooked(ch)      (_isr_mask & (1 << (ch)))
#define _are_hooked(mask)   ((_isr_mask & mask) == mask)
#define _hook(ch)           _isr_mask |= (1 << (ch))
#define _release(ch)        _isr_mask &= ~(1 << (ch))

#define _isr_list_check_irq_owner(ch, _owner) (_isr_list[ch].owner == _owner)

bool __near _isr_list_check_irq_channels_owner(IRQMASK mask, HWOWNER *owner)
{
    IRQMASK m;
    uint8_t i;

    m = _isr_mask & mask;
    i = 0;
    while (m)
    {
        if ((m & 1) && !(_isr_list_check_irq_owner(i, owner)))
            return false;
        m >>= 1;
        i++;
    }

    return true;
}

void __near _isr_list_item_clear(uint8_t ch)
{
    _isr_list[ch].original = NULL;
    _isr_list[ch].owner = NULL;
    _isr_list[ch].handler = NULL;
    _isr_list[ch].data = NULL;
}

void __near _isr_list_item_set_handler(uint8_t ch, ISROWNERCALLBACK *handler, void *data)
{
    _isr_list[ch].handler = handler;
    _isr_list[ch].data = data;
}

void __near _isr_list_item_clear_handler(uint8_t ch)
{
    _isr_list[ch].handler = NULL;
    _isr_list[ch].data = NULL;
}

void __near _isr_list_hook_irq(uint8_t ch, HWOWNER *owner, ISROWNERCALLBACK *handler, void *data)
{
    _isr_list[ch].original = _dos_getvect(picvec(ch));
    _isr_list[ch].owner = owner;
    _isr_list[ch].handler = handler;
    _isr_list[ch].data = data;
    _dos_setvect(picvec(ch), isr_get(ch));
    _hook(ch);
}

void __near _isr_list_release_irq(uint8_t ch)
{
    _release(ch);
    _dos_setvect(picvec(ch), _isr_list[ch].original);
    _isr_list_item_clear(ch);
}

void __far __cdecl _isr_callback(uint8_t ch)
{
    ISROWNERCALLBACK *handler;

    if (_is_hooked(ch))
    {
        handler = _isr_list[ch].handler;
        if (handler)
        {
            handler(_isr_list[ch].data, ch);
            return;
        }
    }
}

/* Public methods */

IRQMASK pic_get_hooked_irq_channels(void)
{
    return _isr_mask;
}

HWOWNERID pic_get_irq_owner(uint8_t ch)
{
    HWOWNER *owner;

    if (ch < IRQ_CHANNELS)
        if (_is_hooked(ch))
        {
            owner = _isr_list[ch].owner;
            if (owner)
                return hwowner_get_id(owner);
        }

    return 0;
}

ISROWNERCALLBACK *pic_get_irq_handler(uint8_t ch)
{
    if (ch < IRQ_CHANNELS)
        if (_is_hooked(ch))
            return _isr_list[ch].handler;

    return NULL;
}

void *pic_get_irq_data(uint8_t ch)
{
    if (ch < IRQ_CHANNELS)
        if (_is_hooked(ch))
            return _isr_list[ch].data;

    return NULL;
}

/* HWOWNER methods */

bool hwowner_enable_irq(HWOWNER *self, uint8_t ch)
{
    return hwowner_enable_irq_channels(self, 1 << ch);
}

bool hwowner_enable_irq_channels(HWOWNER *self, IRQMASK mask)
{
    if (self && mask)
        if (_are_hooked(mask))
            if (_isr_list_check_irq_channels_owner(mask, self))
            {
                _picio_enable_irq_channels(mask);
                return true;
            }

    return false;
}

bool hwowner_disable_irq(HWOWNER *self, uint8_t ch)
{
    return hwowner_disable_irq_channels(self, 1 << ch);
}

bool hwowner_disable_irq_channels(HWOWNER *self, IRQMASK mask)
{
    if (self && mask)
        if (_are_hooked(mask))
            if (_isr_list_check_irq_channels_owner(mask, self))
            {
                _picio_disable_irq_channels(mask);
                return true;
            }

    return false;
}

bool hwowner_eoi(HWOWNER *self, uint8_t ch)
{
    if (self && (ch < IRQ_CHANNELS))
        if (_is_hooked(ch))
            if (_isr_list_check_irq_owner(ch, self))
            {
                if (ch >= 8)
                    _picio_eoi2(OCW2_CMD_NONSPEC_EOI);

                _picio_eoi1(OCW2_CMD_NONSPEC_EOI);
                return true;
            }

    return false;
}

bool hwowner_hook_irq(HWOWNER *self, uint8_t ch, ISROWNERCALLBACK *handler, void *data)
{
    if (self && (ch < IRQ_CHANNELS))
    {
        if (_is_hooked(ch))
        {
            if (_isr_list_check_irq_owner(ch, self))
            {
                _isr_list_item_set_handler(ch, handler, data);
                return true;
            }
        }
        else
        {
            _isr_list_hook_irq(ch, self, handler, data);
            return true;
        }
    }

    return false;
}

bool hwowner_hook_irq_channels(HWOWNER *self, IRQMASK mask, ISROWNERCALLBACK *handler, void *data)
{
    IRQMASK m;
    uint8_t i;

    if (self && mask)
        if (!_are_hooked(mask))
        {
            m = mask;
            i = 0;
            while (m)
            {
                if (m & 1)
                    _isr_list_hook_irq(i, self, handler, data);
                m >>= 1;
                i++;
            }

            return true;
        }

    return false;
}

bool hwowner_set_irq_handler(HWOWNER *self, uint8_t ch, ISROWNERCALLBACK *handler, void *data)
{
    if (self && (ch < IRQ_CHANNELS))
        if (_is_hooked(ch))
            if (_isr_list_check_irq_owner(ch, self))
            {
                _isr_list_item_set_handler(ch, handler, data);
                return true;
            }

    return false;
}

bool hwowner_set_irq_channels_handler(HWOWNER *self, IRQMASK mask, ISROWNERCALLBACK *handler, void *data)
{
    IRQMASK m;
    uint8_t i;

    if (self && mask)
        if (_are_hooked(mask))
            if (_isr_list_check_irq_channels_owner(mask, self))
            {
                m = mask;
                i = 0;
                while (m)
                {
                    if (m & 1)
                        _isr_list_item_set_handler(i, handler, data);
                    m >>= 1;
                    i++;
                }

                return true;
            }

    return false;
}

bool hwowner_clear_irq_handler(HWOWNER *self, uint8_t ch)
{
    if (self && (ch < IRQ_CHANNELS))
        if (_is_hooked(ch))
            if (_isr_list_check_irq_owner(ch, self))
            {
                _isr_list_item_clear_handler(ch);
                return true;
            }

    return false;
}

bool hwowner_clear_irq_channels_handler(HWOWNER *self, IRQMASK mask)
{
    IRQMASK m;
    uint8_t i;

    if (self && mask)
        if (_are_hooked(mask))
            if (_isr_list_check_irq_channels_owner(mask, self))
            {
                m = mask;
                i = 0;
                while (m)
                {
                    if (m & 1)
                        _isr_list_item_clear_handler(i);
                    m >>= 1;
                    i++;
                }

                return true;
            }

    return false;
}

bool hwowner_release_irq(HWOWNER *self, uint8_t ch)
{
    if (self && (ch < IRQ_CHANNELS))
        if (_is_hooked(ch))
            if (_isr_list_check_irq_owner(ch, self))
            {
                _isr_list_release_irq(ch);
                return true;
            }

    return false;
}

bool hwowner_release_irq_channels(HWOWNER *self, IRQMASK mask)
{
    IRQMASK m;
    uint8_t i;

    if (self && mask)
        if (_are_hooked(mask))
            if (_isr_list_check_irq_channels_owner(mask, self))
            {
                m = mask;
                i = 0;
                while (m)
                {
                    if (m & 1)
                        _isr_list_release_irq(i);
                    m >>= 1;
                    i++;
                }

                return true;
            }

    return false;
}

/* Initialization */

void __near pic_init(void)
{
    int i;

    for (i = 0; i < IRQ_CHANNELS; i++)
        _isr_list_item_clear(i);

    _isr_mask = 0;
}

void __near pic_done(void)
{
    IRQMASK m;
    int i;

    m = _isr_mask;
    i = 0;
    while (m)
    {
        if (m & 1)
            _isr_list_release_irq(i);
        m >>= 1;
        i++;
    }
}

DEFINE_REGISTRATION(pic, pic_init, pic_done)

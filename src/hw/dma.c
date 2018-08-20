/* dma.c -- Intel 8237 DMA controller interface.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/conio.h"
#include "cc/dos.h"
#include "cc/stdio.h"
#include "cc/stdlib.h"
#include "cc/string.h"
#include "debug.h"

#include "hw/dma.h"

// TODO: remove PUBLIC_CODE macros when done.

/* I/O ports */

#pragma pack(push, 1);
typedef struct dma_IO_t
{
    uint8_t mask;
    uint8_t clear;
    uint8_t mode;
    uint8_t addr;
    uint8_t page;
    uint8_t count;
};
#pragma pack(pop);

static const struct dma_IO_t _dma_IO[DMA_CHANNELS] =
{
    /* 8-bits transfers */
    { 0x0a, 0x0c, 0x0b, 0x00, 0x87, 0x01 },
    { 0x0a, 0x0c, 0x0b, 0x02, 0x83, 0x03 },
    { 0x0a, 0x0c, 0x0b, 0x04, 0x81, 0x05 },
    { 0x0a, 0x0c, 0x0b, 0x06, 0x82, 0x07 },
    /* 16-bits transfers */
    { 0xd4, 0xd8, 0xd6, 0xc0, 0x8f, 0xc2 },
    { 0xd4, 0xd8, 0xd6, 0xc4, 0x8b, 0xc6 },
    { 0xd4, 0xd8, 0xd6, 0xc8, 0x89, 0xca },
    { 0xd4, 0xd8, 0xd6, 0xcc, 0x8a, 0xce }
};

/* multi channels mask */

/* channels 0-3 */
#define DMAIO_MASKMULTI_0 0x0f
/* channels 4-7 */
#define DMAIO_MASKMULTI_1 0xde

/* multi channels enable */

/* channels 0-3 */
#define DMAIO_ENABLEMULTI_0 0x0e
/* channels 4-7 */
#define DMAIO_ENABLEMULTI_1 0xdc

/* single channel mask / enable */

#define MASK_CHAN 0x03
#define MASK_MASK 0x04

/*** DMA list ***/

/* Private data for DMA list */

static struct
{
    HWOWNER *owner;
} _dma_list[DMA_CHANNELS] = { NULL };

static DMAMASK _dma_mask = 0;

#define _is_hooked(ch)      (_dma_mask & (1 << (ch)))
#define _are_hooked(mask)   ((_dma_mask & mask) == mask)
#define _hook(ch)           _dma_mask |= (1 << (ch))
#define _release(ch)        _dma_mask &= ~(1 << (ch))

/* Private methods for DMA list */

#define _dma_list_check_dma_owner(ch, _owner) (_dma_list[ch].owner == _owner)

bool __near _dma_list_check_dma_channels_owner(DMAMASK mask, HWOWNER *owner)
{
    DMAMASK m;
    uint8_t ch;

    m = _dma_mask & mask;
    ch = 0;
    while (m)
    {
        if ((m & 1) && !(_dma_list_check_dma_owner(ch, owner)))
            return false;
        m >>= 1;
        ch++;
    }

    return true;
}

void __near _dma_list_hook_dma(uint8_t ch, HWOWNER *owner)
{
    _dma_list[ch].owner = owner;
    _hook(ch);
}

void __near _dma_list_release_dma(uint8_t ch)
{
    _release(ch);
    _dma_list[ch].owner = NULL;
}

/* Private I/O methods */

void __near _dmaio_mask(uint8_t ch)
{
    outp(_dma_IO[ch].mask, (ch & MASK_CHAN) | MASK_MASK);
}

void __near _dmaio_mask_channels(DMAMASK mask)
{
    if (mask & 0x0f)
        outp(DMAIO_MASKMULTI_0, mask & 0x0f);

    if (mask & 0xf0)
        outp(DMAIO_MASKMULTI_1, mask >> 4);
}

void __near _dmaio_enable(uint8_t ch)
{
    outp(_dma_IO[ch].mask, ch & MASK_CHAN);
}

void __near _dmaio_enable_channels(DMAMASK mask)
{
    if (mask & 0x0f)
        outp(DMAIO_ENABLEMULTI_0, mask & 0x0f);

    if (mask & 0xf0)
        outp(DMAIO_ENABLEMULTI_1, mask >> 4);
}

void __near _dmaio_setup(uint8_t ch, DMAMODE mode, uint32_t linear, uint16_t count)
{
    uint16_t addr;
    uint8_t page;

    /* clear flip-flop */
    outp(_dma_IO[ch].clear, 0);

    /* set mode */
    outp(_dma_IO[ch].mode, (mode & (~DMA_MODE_CHAN_MASK) | (ch & DMA_MODE_CHAN_MASK)));

    if (ch < 4)
    {
        addr = linear & 0xffff;
        page = (linear >> 16) & 0xff;
    } else {
        /* address is in 16-bit values */
        addr = (linear >> 1) & 0xffff;
        /* page address is the same but now it accesses 128 KiB continously */
        page = (linear >> 16) & 0xfe;
    }
    count--;

    /* set memory address, page, count */
    outp(_dma_IO[ch].addr, addr & 0xff);
    outp(_dma_IO[ch].addr, (addr >> 8) & 0xff);
    outp(_dma_IO[ch].page, page);
    outp(_dma_IO[ch].count, count & 0xff);
    outp(_dma_IO[ch].count, (count >> 8) & 0xff);
}

uint16_t __near _dmaio_get_counter(uint8_t ch)
{
    uint8_t lo, hi;

    /* clear flip-flop */
    outp(_dma_IO[ch].clear, 0);

    lo = inp(_dma_IO[ch].count);
    hi = inp(_dma_IO[ch].count);
    /* bytes|words left to send = result + 1 */

    return lo + (hi << 8);
}

/*** Public methods ***/

uint32_t dma_get_linear_address(void *p)
{
    return ((uint32_t)(FP_SEG(p)) << 4) + FP_OFF(p);
}

DMAMASK dma_get_hooked_channels(void)
{
    return _dma_mask;
}

HWOWNERID dma_get_owner(uint8_t ch)
{
    HWOWNER *owner;

    if (ch < DMA_CHANNELS)
        if (_is_hooked(ch))
        {
            owner = _dma_list[ch].owner;
            if (owner)
                return hwowner_get_id(owner);
        }

    return 0;
}

bool hwowner_hook_dma(HWOWNER *self, uint8_t ch)
{
    if (self && (ch < DMA_CHANNELS))
        if (!_is_hooked(ch))
        {
            _dma_list_hook_dma(ch, self);
            return true;
        }

    return false;
}

bool hwowner_hook_dma_channels(HWOWNER *self, DMAMASK mask)
{
    DMAMASK m;
    uint8_t ch;

    if (self && mask)
        if (!_are_hooked(mask))
        {
            m = mask;
            ch = 0;
            while (m)
            {
                if (m & 1)
                    _dma_list_hook_dma(ch, self);
                m >>= 1;
                ch++;
            }

            return true;
        }

    return false;
}

bool hwowner_mask_dma(HWOWNER *self, uint8_t ch)
{
    if (self && (ch < DMA_CHANNELS))
        if (_is_hooked(ch))
            if (_dma_list_check_dma_owner(ch, self))
            {
                _dmaio_mask(ch);
                return true;
            }

    return false;
}

bool hwowner_mask_dma_channels(HWOWNER *self, DMAMASK mask)
{
    if (self && mask)
        if (_are_hooked(mask))
            if (_dma_list_check_dma_channels_owner(mask, self))
            {
                _dmaio_mask_channels(_dma_mask & mask);
                return true;
            }

    return false;
}

bool hwowner_enable_dma(HWOWNER *self, uint8_t ch)
{
    if (self && (ch < DMA_CHANNELS))
        if (_is_hooked(ch))
            if (_dma_list_check_dma_owner(ch, self))
            {
                _dmaio_enable(ch);
                return true;
            }

    return false;
}

bool hwowner_enable_dma_channels(HWOWNER *self, DMAMASK mask)
{
    if (self && mask)
        if (_are_hooked(mask))
            if (_dma_list_check_dma_channels_owner(mask, self))
            {
                _dmaio_enable_channels(mask);
                return true;
            }

    return false;
}

bool hwowner_setup_dma_transfer(HWOWNER *self, uint8_t ch, DMAMODE mode, uint32_t l, uint16_t count)
{
    if (self && (ch < DMA_CHANNELS))
        if (_is_hooked(ch))
            if (_dma_list_check_dma_owner(ch, self))
            {
                _dmaio_mask(ch);
                _dmaio_setup(ch, mode, l, count);
                _dmaio_enable(ch);
                return true;
            }

    return false;
}

uint16_t hwowner_get_dma_counter(HWOWNER *self, uint8_t ch)
{
    if (self && (ch < DMA_CHANNELS))
        if (_is_hooked(ch))
            if (_dma_list_check_dma_owner(ch, self))
                return _dmaio_get_counter(ch);

    return 0;
}

bool hwowner_release_dma(HWOWNER *self, uint8_t ch)
{
    if (self && (ch < DMA_CHANNELS))
        if (_is_hooked(ch))
            if (_dma_list_check_dma_owner(ch, self))
            {
                _dma_list_release_dma(ch);
                return true;
            }

    return false;
}

bool hwowner_release_dma_channels(HWOWNER *self, DMAMASK mask)
{
    DMAMASK m;
    uint8_t ch;

    if (self && mask)
        if (_are_hooked(mask))
            if (_dma_list_check_dma_channels_owner(mask, self))
            {
                m = mask;
                ch = 0;
                while (m)
                {
                    if (m & 1)
                        _dma_list_release_dma(ch);
                    m >>= 1;
                    ch++;
                }

                return true;
            }

    return false;
}

/* Buffer */

void PUBLIC_CODE dmaBuf_init(DMABUF *self)
{
    if (self)
    {
        self->data = NULL;
        self->size = 0;
        self->unaligned = NULL;
    }
}

bool PUBLIC_CODE dmaBuf_alloc(DMABUF *self, uint32_t size)
{
    uint16_t seg, max;
    uint32_t bufStart, bufEnd, bufSize, dmaStart, dmaEnd, dmaSize;
    dmaSize = size;

    if (self)
    {
        if (!self->data)
        {
            /* 64 KiB max. limit (for 8-bits channel) */
            dmaSize = dmaSize > 0x10000 ? 0x10000 : ((dmaSize + 15) & 0x1fff0);

            bufSize = dmaSize << 1;
            if (_dos_allocmem(_dos_para(bufSize), &seg))
            {
                DEBUG_FAIL("dmaBuf_alloc", "Failed to allocate DOS memory.");
                return false;
            }

            self->unaligned = MK_FP(seg, 0);

            bufStart = dma_get_linear_address(self->unaligned);
            bufEnd = bufStart + bufSize - 1;

            DEBUG_MSG_("dmaBuf_alloc",
                "Allocated %lu bytes of DOS memory for DMA buffer at 0x%05lX-0x%05lX.",
                (uint32_t)bufSize, (uint32_t)bufStart, (uint32_t)bufEnd);

            dmaStart = bufStart;
            dmaEnd = dmaStart + dmaSize - 1;

            if (((uint32_t)dmaStart & 0xf0000) != ((uint32_t)dmaEnd & 0xf0000))
            {
                dmaStart = (bufStart & 0xf0000) + 0x10000;
                dmaEnd = dmaStart + dmaSize - 1;
            }

            self->size = dmaSize;
            self->data = MK_FP(dmaStart >> 4, 0);

            DEBUG_MSG_("dmaBuf_alloc",
                "Using %lu bytes for DMA buffer at 0x%05lX-0x%05lX.",
                (uint32_t)self->size, (uint32_t)dmaStart, (uint32_t)dmaEnd);

            if (dmaEnd < bufEnd)
            {
                DEBUG_MSG_("dmaBuf_alloc",
                    "Freeing unused trailing %lu bytes of allocated DMA buffer...",
                    (uint32_t)(bufEnd - dmaEnd));

                bufSize = dmaEnd - bufStart + 1;
                _dos_setblock(_dos_para(bufSize), FP_SEG(self->unaligned), &max);
            }

            memset(self->data, 0, self->size);

            return true;
        }
        else
        {
            DEBUG_FAIL("dmaBuf_alloc", "Memory is already allocated.");
            return false;
        }
    }
    else
    {
        DEBUG_FAIL("dmaBuf_alloc", "Self is NULL.");
        return false;
    }
}

void PUBLIC_CODE dmaBuf_free(DMABUF *self)
{
    if (self)
    {
        if (self->unaligned)
            _dos_freemem(FP_SEG(self->unaligned));
    }
}

/*** Initialization ***/

void dmaInit(void)
{
    uint8_t i;

    for (i = 0; i < DMA_CHANNELS; i++)
        _dma_list[i].owner = NULL;

    _dma_mask = 0;
}

void dmaDone(void)
{
    uint8_t i;

    for (i = 0; i < DMA_CHANNELS; i++)
        _dma_list[i].owner = NULL;

    _dma_mask = 0;
}

DEFINE_REGISTRATION(dma, dmaInit, dmaDone)

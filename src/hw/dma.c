/* dma.c -- Intel 8237 DMA controller interface.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// TODO: remove PUBLIC_CODE macros when done.

#include "pascal/pascal.h"
#include "cc/i86.h"
#include "cc/conio.h"
#include "cc/dos.h"
#include "cc/stdio.h"
#include "debug.h"

#include "hw/dma.h"

/* I/O ports */

typedef struct dmaIO_t {
    uint8_t mask;
    uint8_t clear;
    uint8_t mode;
    uint8_t addr;
    uint8_t page;
    uint8_t count;
};

static const struct dmaIO_t _dmaIO[DMA_CHANNELS] = {
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

typedef struct dmaInfo_t {
    dmaOwner_t *owner;
};

/* FIXME: to link with Pascal linker we use static definition and local
 *  initialization. Remove these when done. */
static struct dmaInfo_t _dmaList[DMA_CHANNELS] = { (void const *)0 };

/* Private methods for DMA list */

void __near _dmaSetOwner(uint8_t ch, dmaOwner_t *owner)
{
    _dmaList[ch].owner = owner;
}

dmaOwner_t *__near _dmaGetOwner(uint8_t ch)
{
    return _dmaList[ch].owner;
}

bool __near _dmaIsAvailable(uint8_t ch)
{
    return (_dmaGetOwner(ch) != (void *)0);
}

void __near _dmaClearOwner(uint8_t ch)
{
    _dmaSetOwner(ch, (void *)0);
}

void __near _dmaInit(uint8_t ch)
{
    _dmaClearOwner(ch);
}

void __near _dmaDone(uint8_t ch)
{
    _dmaClearOwner(ch);
}

void __near _dmaListInit()
{
    int ch;
    for (ch = 0; ch < DMA_CHANNELS; ch++)
        _dmaInit(ch);
}

void __near _dmaListDone()
{
    int ch;
    for (ch = 0; ch < DMA_CHANNELS; ch++)
        _dmaDone(ch);
}

/*** Private I/O methods ***/

void __near _dmaioMask(uint8_t ch)
{
    outp(_dmaIO[ch].mask, (ch & MASK_CHAN) | MASK_MASK);
}

void __near _dmaioMaskChannels(dmaMask_t mask)
{
    if (mask & 0x0f) outp(DMAIO_MASKMULTI_0, mask & 0x0f);
    if (mask & 0xf0) outp(DMAIO_MASKMULTI_1, mask >> 4);
}

void __near _dmaioEnable(uint8_t ch)
{
    outp(_dmaIO[ch].mask, ch & MASK_CHAN);
}

void __near _dmaioEnableChannels(dmaMask_t mask)
{
    if (mask & 0x0f) outp(DMAIO_ENABLEMULTI_0, mask & 0x0f);
    if (mask & 0xf0) outp(DMAIO_ENABLEMULTI_1, mask >> 4);
}

void __near _dmaioSetup(uint8_t ch, dmaMode_t mode, uint32_t linear, uint16_t count)
{
    uint16_t addr;
    uint8_t page;

    /* clear flip-flop */
    outp(_dmaIO[ch].clear, 0);

    /* set mode */
    outp(_dmaIO[ch].mode, (mode & (~DMA_MODE_CHAN_MASK) | (ch & DMA_MODE_CHAN_MASK)));

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
    outp(_dmaIO[ch].addr, addr & 0xff);
    outp(_dmaIO[ch].addr, (addr >> 8) & 0xff);
    outp(_dmaIO[ch].page, page);
    outp(_dmaIO[ch].count, count & 0xff);
    outp(_dmaIO[ch].count, (count >> 8) & 0xff);
}

uint16_t __near _dmaioGetCounter(uint8_t ch)
{
    uint8_t lo, hi;

    /* clear flip-flop */
    outp(_dmaIO[ch].clear, 0);

    lo = inp(_dmaIO[ch].count);
    hi = inp(_dmaIO[ch].count);
    /* bytes|words left to send = result + 1 */

    return lo + (hi << 8);
}

/*** Public I/O methods ***/

void PUBLIC_CODE dmaMaskSingleChannel(uint8_t ch)
{
    if (ch < DMA_CHANNELS) _dmaioMask(ch);
}

void PUBLIC_CODE dmaMaskChannels(dmaMask_t mask)
{
    _dmaioMaskChannels(mask);
}

void PUBLIC_CODE dmaEnableSingleChannel(uint8_t ch)
{
    if (ch < DMA_CHANNELS) _dmaioEnable(ch);
}

void PUBLIC_CODE dmaEnableChannels(dmaMask_t mask)
{
    _dmaioEnableChannels(mask);
}

uint32_t PUBLIC_CODE dmaGetLinearAddress(void *p)
{
    return ((uint32_t)(FP_SEG(p)) << 4) + FP_OFF(p);
}

void PUBLIC_CODE dmaSetupSingleChannel(uint8_t ch, dmaMode_t mode, uint32_t l, uint16_t count)
{
    if (ch < DMA_CHANNELS)
    {
        _dmaioMask(ch);
        _dmaioSetup(ch, mode, l, count);
        _dmaioEnable(ch);
    };
}

uint16_t PUBLIC_CODE dmaGetCounter(uint8_t ch)
{
    return (ch < DMA_CHANNELS ? _dmaioGetCounter(ch) : 0);
}

/*** Public methods for DMA list ***/

bool PUBLIC_CODE dmaIsAvailableSingleChannel(uint8_t ch)
{
    return (ch < DMA_CHANNELS ? _dmaIsAvailable(ch) : false);
}

dmaMask_t PUBLIC_CODE dmaGetAvailableChannels(void)
{
    dmaMask_t mask;
    int ch;
    mask = 0;
    for (ch = 0; ch < DMA_CHANNELS; ch++)
        if (_dmaIsAvailable(ch))
            mask |= (1 << ch);
    return mask;
}

dmaOwner_t *PUBLIC_CODE dmaGetSingleChannelOwner(uint8_t ch)
{
    return (ch < DMA_CHANNELS ? _dmaGetOwner(ch) : (void *)0);
}

void PUBLIC_CODE dmaHookSingleChannel(uint8_t ch, dmaOwner_t *owner)
{
    if ((ch < DMA_CHANNELS) && _dmaIsAvailable(ch))
        _dmaSetOwner(ch, owner);
}

void PUBLIC_CODE dmaHookChannels(dmaMask_t mask, dmaOwner_t *owner)
{
    int ch;
    for (ch = 0; ch < DMA_CHANNELS; ch++)
        if ((mask & (1 << ch)) && _dmaIsAvailable(ch))
            dmaHookSingleChannel(ch, owner);
}

void PUBLIC_CODE dmaReleaseSingleChannel(uint8_t ch)
{
    if ((ch < DMA_CHANNELS) && (!_dmaIsAvailable(ch)))
        _dmaClearOwner(ch);
}

void PUBLIC_CODE dmaReleaseChannels(dmaMask_t mask)
{
    int ch;
    for (ch = 0; ch < DMA_CHANNELS; ch++)
        if (mask & (1 << ch))
            dmaReleaseSingleChannel(ch);
}

/* Buffer */

DMABUF *PUBLIC_CODE dmaBuf_new(void)
{
    uint16_t seg;
    if (!_dos_allocmem(_dos_para(sizeof(DMABUF)), &seg))
        return MK_FP(seg, 0);
    else
        return (void *)0;
}

void PUBLIC_CODE dmaBuf_delete(DMABUF **buf)
{
    if (buf)
    {
        if (*buf)
        {
            _dos_freemem(FP_SEG(*buf));
            *buf = (void *)0;
        };
    };
}

void __near _dmaBufClear(DMABUF *buf)
{
    buf->data = (void *)0;
    buf->size = 0;
    buf->unaligned = (void *)0;
}

bool PUBLIC_CODE dmaBufAlloc(DMABUF *buf, uint32_t size)
{
    uint16_t seg, max;
    uint32_t bufStart, bufEnd, bufSize, dmaStart, dmaEnd, dmaSize;
    dmaSize = size;

    if (buf && !buf->unaligned)
    {
        /* 64 KiB max. limit (for 8-bits channel) */
        dmaSize = dmaSize > 0x10000 ? 0x10000 : ((dmaSize + 15) & 0x1fff0);

        bufSize = dmaSize << 1;
        if (_dos_allocmem(_dos_para(bufSize), &seg))
            return false;
        buf->unaligned = MK_FP(seg, 0);

        bufStart = dmaGetLinearAddress(buf->unaligned);
        bufEnd = bufStart + bufSize - 1;

        #ifdef DEBUG
        printf("[info] Allocated %lu bytes of DOS memory for DMA buffer at 0x%05lX-0x%05lX\r\n",
            (uint32_t)bufSize, (uint32_t)bufStart, (uint32_t)bufEnd);
        #endif

        dmaStart = bufStart;
        dmaEnd = dmaStart + dmaSize - 1;

        if (((uint32_t)dmaStart & 0xf0000) != ((uint32_t)dmaEnd & 0xf0000)) {
            dmaStart = (bufStart & 0xf0000) + 0x10000;
            dmaEnd = dmaStart + dmaSize - 1;
        }

        buf->size = dmaSize;
        buf->data = MK_FP(dmaStart >> 4, 0);
        #ifdef DEBUG
        printf("[info] Using %lu bytes for DMA buffer at 0x%05lX-0x%05lX\r\n",
            (uint32_t)buf->size, (uint32_t)dmaStart, (uint32_t)dmaEnd);
        #endif

        if (dmaEnd < bufEnd) {
            #ifdef DEBUG
            printf("[info] Freeing unused trailing %lu bytes of allocated DMA buffer\r\n",
                (uint32_t)(bufEnd - dmaEnd));
            #endif
            bufSize = dmaEnd - bufStart + 1;
            _dos_setblock(_dos_para(bufSize), FP_SEG(buf->unaligned), &max);
        }
        memset(buf->data, 0, buf->size);

        return true;
    };
    return false;
}

void PUBLIC_CODE dmaBufFree(DMABUF *buf)
{
    if (buf)
    {
        if (buf->unaligned)
            _dos_freemem(FP_SEG(buf->unaligned));
        _dmaBufClear(buf);
    };
}

void PUBLIC_CODE dmaBufInit(DMABUF *buf)
{
    if (buf)
        _dmaBufClear(buf);
}

void PUBLIC_CODE dmaBufDone(DMABUF *buf)
{
    if (buf)
        dmaBufFree(buf);
}

/*** Initialization ***/

void dmaInit(void)
{
    _dmaListInit();
}

void dmaDone(void)
{
    _dmaListDone();
}

DEFINE_REGISTRATION(dma, dmaInit, dmaDone)

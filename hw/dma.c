/* dma.c -- Intel 8237 DMA controller interface.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <i86.h>
#include <stdbool.h>
#include <stdint.h>
#include <conio.h>
#endif

// TODO: remove PUBLIC_CODE macros when done.

#include "..\pascal\pascal.h"

#include "dma.h"

/* DMA i/o ports */

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

void PUBLIC_CODE dmaMaskSingleChannel(uint8_t ch)
{
    if (ch < DMA_CHANNELS) outp(_dmaIO[ch].mask, (ch & MASK_CHAN) | MASK_MASK);
}

void PUBLIC_CODE dmaMaskChannels(dmaMask_t mask)
{
    if (mask & 0x0f) outp(DMAIO_MASKMULTI_0, mask & 0x0f);
    if (mask & 0xf0) outp(DMAIO_MASKMULTI_1, mask >> 4);
}

void PUBLIC_CODE dmaEnableSingleChannel(uint8_t ch)
{
    if (ch < DMA_CHANNELS) outp(_dmaIO[ch].mask, ch & MASK_CHAN);
}

void PUBLIC_CODE dmaEnableChannels(dmaMask_t mask)
{
    if (mask & 0x0f) outp(DMAIO_ENABLEMULTI_0, mask & 0x0f);
    if (mask & 0xf0) outp(DMAIO_ENABLEMULTI_1, mask >> 4);
}

uint32_t PUBLIC_CODE dmaGetLinearAddress(void *p)
{
    return ((uint32_t)(FP_SEG(p)) << 4) + FP_OFF(p);
}

void PUBLIC_CODE dmaSetupSingleChannel(uint8_t ch, dmaMode_t mode, void *p, uint16_t count)
{
    uint32_t linear = dmaGetLinearAddress(p);
    uint16_t addr;
    uint8_t page;

    if (ch < DMA_CHANNELS)
    {
        dmaMaskSingleChannel(ch);

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

        dmaEnableSingleChannel(ch);
    };
}

uint16_t PUBLIC_CODE dmaGetCounter(uint8_t ch)
{
    uint8_t lo, hi;

    if (ch < DMA_CHANNELS)
    {
        /* clear flip-flop */
        outp(_dmaIO[ch].clear, 0);

        lo = inp(_dmaIO[ch].count);
        hi = inp(_dmaIO[ch].count);
        /* bytes|words left to send = result + 1 */

        return lo + (hi << 8);
    } else {
        return 0;
    };
}

/* Sharing DMA channels */

static dmaOwner_t *_dmaList[DMA_CHANNELS] = { (void const *)0 };

bool PUBLIC_CODE dmaIsAvailableSingleChannel(uint8_t ch)
{
    return (ch < DMA_CHANNELS ? (_dmaList[ch] != (void *)0) : false);
}

dmaMask_t PUBLIC_CODE dmaGetAvailableChannels(void)
{
    dmaMask_t mask;
    int ch;
    mask = 0;
    for (ch = 0; ch < DMA_CHANNELS; ch++)
        if (dmaIsAvailableSingleChannel(ch))
            mask |= (1 << ch);
    return mask;
}

dmaOwner_t *PUBLIC_CODE dmaGetSingleChannelOwner(uint8_t ch)
{
    return (ch < DMA_CHANNELS ? _dmaList[ch] : (void *)0);
}

void __near _dmaSetSingleChannel(uint8_t ch, dmaOwner_t *owner)
{
    _dmaList[ch] = owner;
}

void __near _dmaClearSingleChannel(uint8_t ch)
{
    _dmaSetSingleChannel(ch, (void *)0);
}

void PUBLIC_CODE dmaHookSingleChannel(uint8_t ch, dmaOwner_t *owner)
{
    if (ch < DMA_CHANNELS) _dmaSetSingleChannel(ch, owner);
}

void PUBLIC_CODE dmaHookChannels(dmaMask_t mask, dmaOwner_t *owner)
{
    int ch;
    for (ch = 0; ch < DMA_CHANNELS; ch++)
        if ((mask & (1 << ch)) && dmaIsAvailableSingleChannel(ch))
            _dmaSetSingleChannel(ch, owner);
}

void PUBLIC_CODE dmaReleaseSingleChannel(uint8_t ch)
{
    _dmaClearSingleChannel(ch);
}

void PUBLIC_CODE dmaReleaseChannels(dmaMask_t mask)
{
    int ch;
    for (ch = 0; ch < DMA_CHANNELS; ch++)
        if (mask & (1 << ch))
            dmaReleaseSingleChannel(ch);
}

/* Initialization */

void PUBLIC_CODE dmaInit(void)
{
    int ch;
    for (ch = 0; ch < DMA_CHANNELS; ch++)
        _dmaClearSingleChannel(ch);
}

void PUBLIC_CODE dmaDone(void)
{
    int ch;
    for (ch = 0; ch < DMA_CHANNELS; ch++)
        dmaReleaseSingleChannel(ch);
}

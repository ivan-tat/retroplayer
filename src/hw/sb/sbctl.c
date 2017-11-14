/* sbctl.c -- Sound Blaster hardware control library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/conio.h"
#include "cc/stdio.h"
#include "cc/stdlib.h"
#include "cc/string.h"
#include "cc/errno.h"
#include "hw/dma.h"
#include "hw/pic.h"
#include "hw/sb/sbio.h"
#include "hw/sb/sndisr_.h"
#include "hw/sb/sndisr.h"
#include "hw/sb/detisr_.h"
#include "hw/sb/detisr.h"
#include "debug.h"

#include "hw/sb/sbctl.h"

#define toupper(c) (((c >= 'a') && (c <= 'z')) ? (c - 'a' + 'A') : c)

#define HW_BASE_MAX 8
static const uint16_t HW_BASE_NUM[HW_BASE_MAX] =
{
    0x210, 0x220, 0x230, 0x240, 0x250, 0x260, 0x270, 0x280
};

#define HW_IRQ_MAX 5
static const uint8_t HW_IRQ_NUM[HW_IRQ_MAX] =
{
    2, 3, 5, 7, 10
};

#define HW_DMA_MAX 5
static const uint8_t HW_DMA_NUM[HW_DMA_MAX] =
{
    0, 1, 3, 5, 7
};

typedef uint8_t SBCFGFLAGS;

#define SBCFGFL_TYPE  (1 << 0)
#define SBCFGFL_DSP   (1 << 1)
#define SBCFGFL_IRQ   (1 << 2)
#define SBCFGFL_DMA8  (1 << 3)
#define SBCFGFL_DMA16 (1 << 4)
#define SBCFGFL_BASE_MASK (SBCFGFL_TYPE | SBCFGFL_DSP | SBCFGFL_IRQ | SBCFGFL_DMA8)

static uint8_t  sdev_type;                  /* type */
static char    *sdev_name;                  /* name */
static bool     sdev_configured;            /* sound card is detected */
static bool     sdev_hwflags_base;          /* DSP base I/O address is detected */
static bool     sdev_hwflags_irq;           /* IRQ channel is detected */
static bool     sdev_hwflags_dma8;          /* DMA 8-bits channel is detected */
static bool     sdev_hwflags_dma16;         /* DMA 16-bits channel is detected */
static uint16_t sdev_hw_base;               /* hardware config: DSP base I/O address */
static uint8_t  sdev_hw_irq;                /* hardware config: IRQ channel */
static uint8_t  sdev_hw_dma8;               /* hardware config: DMA channel for 8-bits play */
static uint8_t  sdev_hw_dma16;              /* hardware config: DMA channel for 16-bits play */
static uint16_t sdev_hw_dspv;               /* hardware config: DSP chip version */
static bool     sdev_caps_mixer;            /* capabilities: mixer chip is present */
static bool     sdev_caps_16bits;           /* 16-bits play is possible */
static bool     sdev_caps_stereo;           /* stereo play is possible */
static uint16_t sdev_caps_mono_maxrate;     /* max mono sample rate */
static uint16_t sdev_caps_stereo_maxrate;   /* max stereo sample rate */
static uint8_t  sdev_irq_answer;            /* for detecting */
static void    *sdev_irq_savedvec;          /* for detecting */

static SoundHWISRCallback_t *ISRUserCallback;

static const uint8_t _sb_silence_u8 = 0x80;

void __far __pascal ISRDetectCallback(uint8_t irq)
{
    _disable();

    sbioDSPAcknowledgeIRQ(sdev_hw_base, false);

    if (irq >= 8)
        picEOI(8);  /* secondary */

    picEOI(0);      /* primary */

    sdev_irq_answer = irq;

    _enable();
}

void __far __pascal ISRSoundPlayback(void)
{
    _disable();

    sbioDSPAcknowledgeIRQ(sdev_hw_base, sdev_mode_16bits);

    if (sdev_hw_irq >= 8)
        picEOI(8);  /* secondary */

    picEOI(0);  /* primary */

    if (ISRUserCallback)
        ISRUserCallback();

    _enable();
}

void __near _sb_set_hw(uint8_t type, char *name, bool f_mixer, bool f_16bits, bool f_stereo, uint16_t rate_mono, uint16_t rate_stereo)
{
    sdev_type = type;
    sdev_name = name;
    sdev_caps_mixer = f_mixer;
    sdev_caps_16bits = f_16bits;
    sdev_caps_stereo = f_stereo;
    sdev_caps_mono_maxrate = rate_mono;
    sdev_caps_stereo_maxrate = rate_stereo;
}

void __near _sb_set_hw_dsp(uint8_t type, uint16_t dspv)
{
    switch (type)
    {
    case 1:
        _sb_set_hw(1, "SoundBlaster 1.x", false, false, false, 22050, 0);
        break;
    case 2:
        _sb_set_hw(3, "SoundBlaster 2.x", true, false, false, 44100, 0);
        break;
    case 3:
        _sb_set_hw(2, "SoundBlaster Pro", true, true, false, 44100, 22700);
        break;
    case 4:
        _sb_set_hw(6, "SoundBlaster 16/ASP", true, true, true, 45454, 45454);
        break;
    default:
        _sb_set_hw(0, NULL, false, false, false, 0, 0);
        break;
    }

    sdev_hw_dspv = dspv;
}

void __near _sb_set_hw_flags(bool base, bool irq, bool dma8, bool dma16)
{
    sdev_hwflags_base = base;
    sdev_hwflags_irq = irq;
    sdev_hwflags_dma8 = dma8;
    sdev_hwflags_dma16 = dma16;
}

void __near _sb_set_hw_config(uint16_t base, uint8_t irq, uint8_t dma8, uint8_t dma16)
{
    sdev_hw_base = base;
    sdev_hw_irq = irq;
    sdev_hw_dma8 = dma8;
    sdev_hw_dma16 = dma16;
}

void __near _sb_set_mode(uint16_t rate, bool f_16bits, bool f_signed, bool f_stereo)
{
    sdev_mode_rate = rate;
    sdev_mode_16bits = f_16bits;
    sdev_mode_signed = f_signed;
    sdev_mode_stereo = f_stereo;
}

/* This routine may not work for all registers because of different timings. */
void __near sbMixerWrite(uint8_t reg, uint8_t data)
{
    if (sdev_caps_mixer) sbioMixerWrite(sdev_hw_base, reg, data);
}

uint8_t __near sbMixerRead(uint8_t reg)
{
    if (sdev_caps_mixer)
        return sbioMixerRead(sdev_hw_base, reg);
    else
        return 0;
}

void PUBLIC_CODE sb_set_volume(uint8_t value)
{
    uint8_t b;

    if (sdev_caps_mixer)
    {
        if (sdev_type == 6)
        {
            sbMixerWrite(SBIO_MIXER_MASTER_LEFT, value);
            sbMixerWrite(SBIO_MIXER_MASTER_RIGHT, value);
            sbMixerWrite(SBIO_MIXER_VOICE_LEFT, value);
            sbMixerWrite(SBIO_MIXER_VOICE_RIGHT, value);
        }
        else
        {
            if (value > 15)
                value = 15;
            value |= value << 4;
            sbMixerWrite(SBIO_MIXER_MASTER_VOLUME, value);
            sbMixerWrite(SBIO_MIXER_DAC_LEVEL, value);
        }
    }
}

uint16_t __near _sb_read_DSP_version(void)
{
    uint8_t v_lo, v_hi;

    /* DSP 0xE1 - get DSP version */
    if (! sbioDSPWrite(sdev_hw_base, 0xe1)) return 0;

    v_hi = sbioDSPRead(sdev_hw_base);
    if (sbioError != E_SBIO_SUCCESS) return 0;

    v_lo = sbioDSPRead(sdev_hw_base);
    if (sbioError != E_SBIO_SUCCESS) return 0;

    return v_lo + (v_hi << 8);
}

void __near _sb_set_speaker(bool state)
{
    if (state) {
        /* Does not work on SB16 */
        sbioDSPWrite(sdev_hw_base, 0xd1);
        /* Needs a bit time to switch it on */
        delay(110);
    } else {
        sbioDSPWrite(sdev_hw_base, 0xd3);
        /* Needs a bit time to switch it off */
        delay(220);
    }
}

void __near _sb_adjust_rate(uint16_t *rate, bool stereo, uint8_t *tc)
{
    if ((sdev_type == 6) || ! stereo) {
        *tc = 256 - 1000000 / *rate;
        *rate = 1000000 / (256 - *tc);
    } else {
        *tc = 256 - 1000000 / (2 * *rate);
        *rate = (1000000 / (256 - *tc)) / 2;
    }
}

void PUBLIC_CODE sbAdjustMode(uint16_t *rate, bool *stereo, bool *_16bits) {
    uint8_t tc;

    *stereo = *stereo & sdev_caps_stereo;
    *_16bits = *_16bits & sdev_caps_16bits;
    if (*stereo) {
        if (*rate < 4000) *rate = 4000;
        if (*rate > sdev_caps_stereo_maxrate) *rate = sdev_caps_stereo_maxrate;
    } else {
        if (*rate < 4000) *rate = 4000;
        if (*rate > sdev_caps_mono_maxrate) *rate = sdev_caps_mono_maxrate;
    }
    _sb_adjust_rate(rate, *stereo, &tc);
}

void __near _sb_set_DSP_time_constant(const uint8_t tc)
{
    sbioDSPWrite(sdev_hw_base, 0x40);
    sbioDSPWrite(sdev_hw_base, tc);
}

void __near _sb_set_DSP_rate(const uint16_t rate)
{
    sbioDSPWrite(sdev_hw_base, 0x41);
    sbioDSPWrite(sdev_hw_base, rate >> 8);
    sbioDSPWrite(sdev_hw_base, rate & 0xff);
}

void PUBLIC_CODE sbSetupMode(uint16_t freq, bool stereo)
{
    uint8_t tc;

    DEBUG_BEGIN("sbSetupMode");

    sbioDSPReset(sdev_hw_base);

    /* Calculate time constant and adjust rate
       For SB PRO we have to setup double samplerate in stereo mode */
    _sb_adjust_rate(&freq, stereo, &tc);

    /* Set DSP time constant or frequency */
    if (sdev_type == 6)
        _sb_set_DSP_rate(freq);
    else
        _sb_set_DSP_time_constant(tc);

    /* Setup stereo option for SB PRO
       For SB16 it's set in DSP command */
    if (stereo & (sdev_type != 6))
        sbMixerWrite(0x0e, sbMixerRead(0x0e) || 0x02);

    /* Switch filter option off for SB PRO */
    if (sdev_caps_mixer)
        sbMixerWrite(0x0e, sbMixerRead(0x0e) || 0x20);

    _sb_set_speaker(true);

    DEBUG_END("sbSetupMode");
}

void PUBLIC_CODE sbSetupDSPTransfer(uint16_t len, bool autoinit)
{
    uint8_t cmd, mode;

    DEBUG_BEGIN("sbSetupDSPTransfer");

    if (sdev_type == 6)
    {
        len--;
        if (sdev_mode_16bits) {
            /* DSP 0xB6 - use 16bit autoinit */
            /* DSP 0xB2 - use 16bit nonautoinit */
            cmd = autoinit ? 0xb6 : 0xb2;
        } else {
            /* DSP 0xC6 - use 8bit autoinit */
            /* DSP 0xC2 - use 8bit nonautoinit */
            cmd = autoinit ? 0xc6 : 0xc2;
        }
        sbioDSPWrite(sdev_hw_base, cmd);
        mode = 0;
        /* 2nd command byte: bit 4 = 1 - signed data */
        if (sdev_mode_signed) mode |= 0x10;
        /* 2nd command byte: bit 5 = 1 - stereo data */
        if (sdev_mode_stereo) mode |= 0x20;
        sbioDSPWrite(sdev_hw_base, mode);
        sbioDSPWrite(sdev_hw_base, len & 0xff);
        sbioDSPWrite(sdev_hw_base, (len >> 8) & 0xff);
    } else {
        len--;
        /* DSP 0x48 - setup DMA buffer size */
        sbioDSPWrite(sdev_hw_base, 0x48);
        sbioDSPWrite(sdev_hw_base, len & 0xff);
        sbioDSPWrite(sdev_hw_base, (len >> 8) & 0xff);
        if (sdev_type == 1) {
            /* for SB1.0 : */
            /* DSP 0x1C - autoinit normal DMA */
            /* DSP 0x14 - nonautoinit normal DMA */
            cmd = autoinit ? 0x1c : 0x14;
        } else {
            /* >SB1.0 use highspeed modes */
            /* DSP 0x90 - autoinit highspeed DMA */
            /* DSP 0x91 - nonautoinit highspeed DMA */
            cmd = autoinit ? 0x90 : 0x91;
        }
        sbioDSPWrite(sdev_hw_base, cmd);
    }

    DEBUG_END("sbSetupDSPTransfer");
}

void PUBLIC_CODE sbSetupDMATransfer(void *p, uint16_t count, bool autoinit)
{
    dmaMode_t mode;

    DEBUG_BEGIN("sbSetupDMATransfer");

    mode = DMA_MODE_TRAN_READ | DMA_MODE_ADDR_INCR | DMA_MODE_SINGLE;
    mode |= autoinit ? DMA_MODE_INIT_AUTO : DMA_MODE_INIT_SINGLE;

    dmaSetupSingleChannel(sdev_mode_16bits ? sdev_hw_dma16 : sdev_hw_dma8,
        mode, dmaGetLinearAddress(p), count);

    DEBUG_END("sbSetupDMATransfer");
}

uint16_t PUBLIC_CODE sbGetDMACounter(void) {
    return dmaGetCounter(sdev_mode_16bits ? sdev_hw_dma16 : sdev_hw_dma8);
}

void PUBLIC_CODE pause_play(void) {
    if (sdev_mode_16bits)
        sbioDSPWrite(sdev_hw_base, 0xd5);
    else
        sbioDSPWrite(sdev_hw_base, 0xd0);
}

void PUBLIC_CODE continue_play(void) {
    if (sdev_mode_16bits)
        sbioDSPWrite(sdev_hw_base, 0xd6);
    else
        sbioDSPWrite(sdev_hw_base, 0xd4);
}

void PUBLIC_CODE stop_play(void) {
    /* for 16bit modes : */
    sbioDSPWrite(sdev_hw_base, 0xd0);
    sbioDSPWrite(sdev_hw_base, 0xd9);
    sbioDSPWrite(sdev_hw_base, 0xd0);
    /* for 8bit modes : */
    sbioDSPWrite(sdev_hw_base, 0xd0);
    sbioDSPWrite(sdev_hw_base, 0xda);
    sbioDSPWrite(sdev_hw_base, 0xd0);
    /* reset is the best way to make sure SB stops playing */
    sbioDSPReset(sdev_hw_base);

    dmaMaskSingleChannel(sdev_hw_dma8); /* was outp(0x0a, dma_channel) */

    _sb_set_speaker(false);
}

void PUBLIC_CODE Initblaster(bool *f_16bits, bool *f_stereo, uint16_t *rate)
{
    DEBUG_BEGIN("Initblaster");
    /* first reset SB: */
    sbioDSPAcknowledgeIRQ(sdev_hw_base, false);
    sbioDSPAcknowledgeIRQ(sdev_hw_base, true);
    stop_play();
    /* Now init: */
    sbAdjustMode(rate, f_stereo, f_16bits);                 /* FIXME: +signed */
    _sb_set_mode(*rate, *f_16bits, *f_16bits, *f_stereo);   /* FIXME: *signed */
    sbSetupMode(*rate, *f_stereo);                          /* FIXME: +16bits, +signed */
    DEBUG_END("Initblaster");
}

bool PUBLIC_CODE Detect_DSP_Addr(void)
{
    uint16_t p;
    uint16_t i;

    DEBUG_BEGIN("Detect_DSP_Addr");

    if (sdev_hwflags_base)
    {
        DEBUG_SUCCESS("Detect_DSP_Addr");
        return true;
    }

    i = 0;
    while ((!sdev_hwflags_base) && (i < HW_BASE_MAX))
    {
        p = HW_BASE_NUM[i];
        DEBUG_MSG_("Detect_DSP_Addr", " - probing port 0x%04X...", p);

        if (sbioDSPReset(p))
        {
            sdev_hw_base = p;
            sdev_hwflags_base = true;
        }

        i++;
    }

    if (!sdev_hwflags_base)
    {
        DEBUG_FAIL("Detect_DSP_Addr", "DSP not found.");
        return false;
    }

    #ifdef DEBUG
    if (sdev_hwflags_base)
        DEBUG_INFO_("Detect_DSP_Addr", "Found DSP at base port 0x04X.", p);
    #endif

    sdev_hw_dspv = _sb_read_DSP_version();
    if (sbioError != E_SBIO_SUCCESS)
    {
        DEBUG_FAIL("Detect_DSP_Addr", "Unable to get DSP chip version.");
        return false;
    }

    if (((sdev_hw_dspv >> 8) < 1) || ((sdev_hw_dspv >> 8) > 4))
    {
        DEBUG_FAIL("Detect_DSP_Addr", "Unknown DSP chip version.");
        return false;
    }

    DEBUG_SUCCESS("Detect_DSP_Addr");
    return true;
}

bool __near _sb_detect_IRQ(uint8_t dma, bool f_16bits)
{
    uint16_t rate;
    bool f_stereo;

    DEBUG_BEGIN("_sb_detect_IRQ");

    sdev_irq_answer = 0;

    if (f_16bits)
        sdev_hw_dma16 = dma;
    else
        sdev_hw_dma8 = dma;

    rate = 8000;
    f_stereo = false;
    dmaMaskSingleChannel(dma);
    stop_play();
    Initblaster(&f_16bits, &f_stereo, &rate);
    sbSetupDMATransfer((void *)&_sb_silence_u8, 1, false);
    sbSetupDSPTransfer(1, false);
    delay(10);

    if (sdev_irq_answer)
    {
        #ifdef DEBUG
        DEBUG_INFO("_sb_detect_IRQ", "Found");
        #endif
        DEBUG_SUCCESS("_sb_detect_IRQ");

        if (f_16bits)
            sdev_hwflags_dma16 = true;
        else
            sdev_hwflags_dma8 = true;

        sdev_hw_irq = sdev_irq_answer;
        sdev_hwflags_irq = true;
        return true;
    }
    else
    {
        DEBUG_FAIL("_sb_detect_IRQ", "No DMA and IRQ channels were found.");
        return false;
    }
}

bool PUBLIC_CODE Detect_DMA_Channel_IRQ(void)
{
    void *oldv[HW_IRQ_MAX];
    uint8_t i;
    uint8_t dmac;
    uint8_t dmamask;
    uint8_t irq;
    uint16_t irqmask;

    DEBUG_BEGIN("Detect_DMA_Channel_IRQ");

    if (sdev_hwflags_dma8)
    {
        DEBUG_SUCCESS("Detect_DMA_Channel_IRQ");
        return true;
    }
    if (!sdev_hwflags_base)
    {
        DEBUG_FAIL("Detect_DMA_Channel_IRQ", "DSP base port is not set.");
        return false;
    }

    dmamask = 0;
    for (i = 0; i < HW_DMA_MAX; i++)
        dmamask |= 1 << HW_DMA_NUM[i];

    dmaMaskChannels(dmamask);

    _enable();

    SetDetISRCallback(&ISRDetectCallback);

    irqmask = 0;
    for (i = 0; i < HW_IRQ_MAX; i++)
    {
        irq = HW_IRQ_NUM[i];
        irqmask |= 1 << irq;
        oldv[i] = picGetISR(irq);
        picSetISR(irq, GetDetISR(irq));
    }
    /* no changes for IRQ 2 */
    irqmask &= ~(1 << 2);

    picDisableChannels(irqmask);

    i = 0;
    while ((!sdev_hwflags_dma8) && (i < HW_DMA_MAX))
    {
        dmac = HW_DMA_NUM[i];

        DEBUG_MSG_("Detect_DMA_Channel_IRQ", "- trying DMA channel %hu...", dmac);

        _sb_detect_IRQ(dmac, false);

        i++;
    }

    for (i = 0; i < HW_IRQ_MAX; i++)
        picSetISR(HW_IRQ_NUM[i], oldv[i]);

    picEnableChannels(irqmask);

    if (!sdev_hwflags_dma8)
    {
        DEBUG_FAIL("Detect_DMA_Channel_IRQ", "DMA and IRQ channels were not found.");
        return false;
    }

    sbioDSPReset(sdev_hw_base);

    DEBUG_SUCCESS("Detect_DMA_Channel_IRQ");
    return true;
}

bool PUBLIC_CODE DetectSoundblaster(void)
{
    DEBUG_BEGIN("DetectSoundblaster");

    sdev_configured = false;
    _sb_set_hw_flags(false, false, false, false);
    _sb_set_hw_dsp(0, 0);
    sdev_mode_stereo = false;
    sdev_mode_16bits = false;

    if (!Detect_DSP_Addr())
    {
        DEBUG_FAIL("DetectSoundblaster", "No DSP base I/O address specified.");
        return false;
    }

    /* for the first set SB1.0 - should work on all SBs */
    _sb_set_hw_dsp(1, 0x100);

    stop_play();

    if (!Detect_DMA_Channel_IRQ())
    {
        DEBUG_FAIL("DetectSoundblaster", "Failed to find DMA and IRQ channels.");
        sdev_type = 0;
        return false;
    }

    sbioDSPReset(sdev_hw_base);

    _sb_set_hw_dsp(sdev_hw_dspv >> 8, sdev_hw_dspv);

    if (sdev_type != 0)
    {
        DEBUG_SUCCESS("DetectSoundblaster");
        return true;
    }
    else
    {
        DEBUG_FAIL("DetectSoundblaster", "Unable to detect SoundBlaster.");
        return false;
    }
}

void PUBLIC_CODE set_ready_irq(void *p)
{
    ISRUserCallback = p;
    SetSoundHWISRCallback(&ISRSoundPlayback);
    sdev_irq_savedvec = picGetISR(sdev_hw_irq);
    picSetISR(sdev_hw_irq, GetSoundHWISR());
    /* no changes for IRQ2 */
    picDisableChannels((1 << sdev_hw_irq) & ~(1 << 2));
}

void PUBLIC_CODE restore_irq(void)
{
    /* no changes for IRQ2 */
    picEnableChannels((1 << sdev_hw_irq) & ~(1 << 2));
    picSetISR(sdev_hw_irq, sdev_irq_savedvec);
}

void PUBLIC_CODE Forceto(uint8_t type, uint8_t dma8, uint8_t dma16, uint8_t irq, uint16_t dsp)
{
    switch (type)
    {
    case 1:
        sdev_configured = true;
        _sb_set_hw_dsp(1, 0x100);
        break;
    case 3:
        sdev_configured = true;
        _sb_set_hw_dsp(2, 0x200);
        break;
    case 2:
    case 4:
    case 5:
        sdev_configured = true;
        _sb_set_hw_dsp(3, 0x300);
        break;
    case 6:
        sdev_configured = true;
        _sb_set_hw_dsp(4, 0x400);
        break;
    default:
        sdev_configured = false;
        _sb_set_hw_dsp(0, 0);
        break;
    }

    if (sdev_configured)
    {
        _sb_set_hw_flags(true, true, true, false);
        _sb_set_hw_config(dsp, irq, dma8, dma16);
    }

    _sb_set_mode(0, false, false, false);
}

void PUBLIC_CODE writelnSBConfig(void)
{
    char *s;

    switch (sdev_type)
    {
    case 1:
        s = "Soundblaster 1.x (8 bits @ 24kHz mono)";
        break;
    case 2:
        s = "Soundblaster Pro (8 bits @ 44kHz mono/22kHz stereo)";
        break;
    case 3:
        s = "Soundblaster 2.x (8 bits @ 44kHz mono)";
        break;
    case 4:
        s = "Soundblaster Pro 3.0/4.0 (8 bits @ 44kHz mono/22kHz stereo)";
    case 5:
        s = "Soundblaster Pro<microchannel> (8 bits @ 44kHz mono/22kHz stereo)";
        break;
    case 6: s = "Soundblaster 16/16 ASP (8/16 bits @ 45kHz mono/stereo)";
        break;
    default:
        s = "none";
        break;
    }

    printf(
        "Sound device: %s." CRLF
        "Hardware DSP base I/O address: 0x%04X." CRLF
        "Hardware IRQ channel: %hu." CRLF,
        "Hardware 8-bits DMA channel: %hu." CRLF,
        s, sdev_hw_base, sdev_hw_irq, sdev_hw_dma8
    );
    if (sdev_type = 6)
        printf("Hardware 16-bits DMA channel: %hu." CRLF, sdev_hw_dma16);
}

bool __near _check_value_type(long int v)
{
    return v <= 6;
}

bool __near _check_value_dsp(long int v)
{
    uint16_t i;

    i = 0;
    while (i < HW_BASE_MAX && HW_BASE_NUM[i] != v)
        i++;
    
    return i < HW_BASE_MAX;
}

bool __near _check_value_irq(long int v)
{
    uint16_t i;

    i = 0;
    while (i < HW_IRQ_MAX && HW_IRQ_NUM[i] != v)
        i++;
    
    return i < HW_IRQ_MAX;
}

bool __near _check_value_dma(long int v)
{
    uint16_t i;

    i = 0;
    while (i < HW_DMA_MAX && HW_DMA_NUM[i] != v)
        i++;
    
    return i < HW_DMA_MAX;
}

bool __near _select_type(uint8_t *v)
{
    char c;

    printf(CRLF
        "Select type:" CRLF
        "0) none " CRLF
        "1) SoundBlaster 1.x  ........  8 bits @ 24kHz mono" CRLF
        "2) SoundBlaster 2.x  ........  8 bits @ 44kHz mono" CRLF
        "3) SoundBlaster Pro  ........  8 bits @ 22kHz stereo" CRLF
        "4) SoundBlaster 16/16 ASP ... 16 bits @ 45kHz stereo" CRLF
    );

    do
    {
        c = getch();
    } while (!(c >= '0' && c <= '4'));

    c -= '0';

    switch (c)
    {
    case 0:
        return false;
    case 1:
        *v = 1;
        break;
    case 2:
        *v = 3;
        break;
    case 3:
        *v = 4;  // also 2, 5
        break;
    case 4:
        *v = 6;
        break;
    default:
        break;
    }

    printf("Selected type = %hu." CRLF, *v);
    return true;
}

bool __near _select_DSP(uint16_t *v)
{
    char c;

    printf(CRLF
        "Select DSP base I/O address:" CRLF
        "0) cancel" CRLF
    );

    for (c = 0; c < HW_BASE_MAX; c++)
        printf("%hu) 0x%04X", c, HW_BASE_NUM[c]);
    do
    {
        c = getch();
    }
    while (!(c >= '0' && c <= '0' + HW_BASE_MAX));

    c -= '0';

    if (!c)
        return false;

    *v = HW_BASE_NUM[c - 1];

    printf("Selected DSP base I/O address = 0x%04X." CRLF, *v);

    return true;
}

bool __near _select_IRQ(uint8_t *v)
{
    char c;

    printf(CRLF
        "Select IRQ channel:" CRLF
        "0) cancel" CRLF
    );

    for (c = 0; c < HW_IRQ_MAX; c++)
        printf("%hu) 0x%04X", c, HW_IRQ_NUM[c]);
    do
    {
        c = getch();
    }
    while (!(c >= '0' && c <= '0' + HW_IRQ_MAX));

    c -= '0';

    if (!c)
        return false;

    *v = HW_IRQ_NUM[c - 1];

    printf("Selected IRQ channel = %hu." CRLF, *v);

    return true;
}

bool __near _select_DMA(uint8_t *v, uint8_t bits)
{
    char c;

    printf(CRLF
        "Select %hu-bits DMA channel:" CRLF
        "0) cancel" CRLF,
        bits
    );

    for (c = 0; c < HW_DMA_MAX; c++)
        printf("%hu) 0x%04X", c, HW_DMA_NUM[c]);
    do
    {
        c = getch();
    }
    while (!(c >= '0' && c <= '0' + HW_DMA_MAX));

    c -= '0';

    if (!c)
        return false;

    *v = HW_DMA_NUM[c - 1];

    printf("Selected %hu-bits DMA channel = %hu." CRLF, bits, *v);

    return true;
}

bool PUBLIC_CODE InputSoundblasterValues(void)
{
    uint8_t type, irq, dma8, dma16;
    uint16_t dsp;

    DEBUG_BEGIN("InputSoundblasterValues");

    if (!_select_type(&type))
    {
        DEBUG_FAIL("InputSoundblasterValues", "Cancelled");
        return false;
    }

    if (!_select_DSP(&dsp))
    {
        DEBUG_FAIL("InputSoundblasterValues", "Cancelled");
        return false;
    }

    if (!_select_IRQ(&irq))
    {
        DEBUG_FAIL("InputSoundblasterValues", "Cancelled");
        return false;
    }

    if (!_select_DMA(&dma8, 8))
    {
        DEBUG_FAIL("InputSoundblasterValues", "Cancelled");
        return false;
    }

    if (type == 6)
    {
        if (!_select_DMA(&dma16, 16))
        {
            DEBUG_FAIL("InputSoundblasterValues", "Cancelled");
            return false;
        }
    }
    else
        dma16 = -1;

    Forceto(type, dma8, dma16, irq, dsp);

    DEBUG_SUCCESS("InputSoundblasterValues");
    return true;
}

bool PUBLIC_CODE UseBlasterEnv(void)
{
    char s[256], *param, *endptr;
    uint8_t type, irq, dma8, dma16;
    uint16_t dsp;
    uint16_t len, i;
    SBCFGFLAGS flags;
    long int v;

    DEBUG_BEGIN("UseBlasterEnv");

    _sb_set_hw_dsp(0, 0);
    _sb_set_hw_flags(false, false, false, false);
    _sb_set_hw_config(-1, -1, -1, -1);
    _sb_set_mode(0, false, false, false);

    custom_getenv(s, "BLASTER", 255);
    len = strlen(s);
    if (!len)
    {
        DEBUG_FAIL("UseBlasterEnv", "BLASTER environment variable is not set.");
        return false;
    }
    for (i = 0; i < len; i++)
        s[i] = toupper(s[i]);

    flags = 0;

    /* BLASTER=Annn In Dn Hn Pnnn Tn */

    param = strchr(s, 'T');
    if (param)
    {
        errno = 0;
        v = strtol(param + 1, &endptr, 10);
        if ((!errno) && _check_value_type(v))
        {
            DEBUG_INFO_("UseBlasterEnv", "Type=%u", (uint16_t)v);
            type = v;
            flags |= SBCFGFL_TYPE;
        }
    }

    param = strchr(s, 'A');
    if (param)
    {
        errno = 0;
        v = strtol(param + 1, &endptr, 16);
        if ((!errno) && _check_value_dsp(v))
        {
            DEBUG_INFO_("UseBlasterEnv", "DSP=0x%04X", (uint16_t)v);
            dsp = v;
            flags |= SBCFGFL_DSP;
        }
    }

    param = strchr(s, 'I');
    if (param)
    {
        errno = 0;
        v = strtol(param + 1, &endptr, 10);
        if ((!errno) && _check_value_irq(v))
        {
            DEBUG_INFO_("UseBlasterEnv", "IRQ=%hu", (uint8_t)v);
            irq = v;
            flags |= SBCFGFL_IRQ;
        }
    }

    param = strchr(s, 'D');
    if (param)
    {
        errno = 0;
        v = strtol(param + 1, &endptr, 10);
        if ((!errno) && _check_value_dma(v))
        {
            DEBUG_INFO_("UseBlasterEnv", "DMA8=%hu", (uint8_t)v);
            dma8 = v;
            flags |= SBCFGFL_DMA8;
        }
    }

    param = strchr(s, 'H');
    if (param)
    {
        errno = 0;
        v = strtol(param + 1, &endptr, 10);
        if ((!errno) && _check_value_dma(v))
        {
            DEBUG_INFO_("UseBlasterEnv", "DMA16=%hu", (uint8_t)v);
            dma16 = v;
            flags |= SBCFGFL_DMA16;
        }
    }

    if (flags & SBCFGFL_BASE_MASK == SBCFGFL_BASE_MASK)
    {
        Forceto(type, dma8, dma16, irq, dsp);
    }
    else
    {
        DEBUG_FAIL("UseBlasterEnv", "Configuration string is not complete.");
        return false;
    }

    DEBUG_SUCCESS("UseBlasterEnv");
    return true;
}

/*** Initialization ***/

void sbctl_init(void)
{
    sdev_configured = false;
    _sb_set_hw_flags(false, false, false, false);
    _sb_set_hw_config(0x220, 7, 1, 5);
    _sb_set_hw_dsp(0, 0);
    _sb_set_mode(0, false, false, false);
    ISRUserCallback = NULL;
}

void sbctl_done(void)
{
}

DEFINE_REGISTRATION(sbctl, sbctl_init, sbctl_done)

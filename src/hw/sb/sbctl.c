/* sbctl.c -- Sound Blaster hardware control library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal/pascal.h"
#include "cc/i86.h"
#include "cc/conio.h"
#include "hw/dma.h"
#include "hw/sb/sbio.h"
#include "hw/sb/sbctl.h"

/* This routine may not work for all registers because of different timings. */
void sbMixerWrite(uint8_t reg, uint8_t data) {
    if (sdev_caps_mixer) sbioMixerWrite(sdev_hw_base, reg, data);
}

uint8_t sbMixerRead(uint8_t reg) {
    if (sdev_caps_mixer)
        return sbioMixerRead(sdev_hw_base, reg);
    else
        return 0;
}

void PUBLIC_CODE setvolume(uint8_t vol) {
    uint8_t b;

    if (sdev_caps_mixer) {
        if (sbno == 6) {
            sbMixerWrite(SBIO_MIXER_MASTER_LEFT, vol);
            sbMixerWrite(SBIO_MIXER_MASTER_RIGHT, vol);
            sbMixerWrite(SBIO_MIXER_VOICE_LEFT, vol);
            sbMixerWrite(SBIO_MIXER_VOICE_RIGHT, vol);
        } else {
            if (vol > 15) vol = 15;
            vol |= vol << 4;
            sbMixerWrite(SBIO_MIXER_MASTER_VOLUME, vol);
            sbMixerWrite(SBIO_MIXER_DAC_LEVEL, vol);
        }
    }
}

uint16_t PUBLIC_CODE sbReadDSPVersion(void) {
    uint8_t v_lo, v_hi;

    /* DSP 0xE1 - get DSP version */
    if (! sbioDSPWrite(sdev_hw_base, 0xe1)) return 0;

    v_hi = sbioDSPRead(sdev_hw_base);
    if (sbioError != E_SBIO_SUCCESS) return 0;

    v_lo = sbioDSPRead(sdev_hw_base);
    if (sbioError != E_SBIO_SUCCESS) return 0;

    return v_lo + (v_hi << 8);
}

void setSpeaker(bool state) {
    if (state) {
        /* Does not work on SB16 */
        sbioDSPWrite(sdev_hw_base, 0xd1);
        /* Needs a bit time to switch it on */
        pascal_delay(110);
    } else {
        sbioDSPWrite(sdev_hw_base, 0xd3);
        /* Needs a bit time to switch it off */
        pascal_delay(220);
    }
}

void adjustRate(uint16_t *rate, bool stereo, uint8_t *tc) {
    if ((sbno == 6) || ! stereo) {
        *tc = 256 - 1000000 / *rate;
        *rate = 1000000 / (256 - *tc);
    } else {
        *tc = 256 - 1000000 / (2 * *rate);
        *rate = (1000000 / (256 - *tc)) / 2;
    }
}

void PUBLIC_CODE sbAdjustMode(uint16_t *rate, bool *stereo, bool *_16bit) {
    uint8_t tc;

    *stereo = *stereo & sdev_caps_stereo;
    *_16bit = *_16bit & sdev_caps_16bit;
    if (*stereo) {
        if (*rate < 4000) *rate = 4000;
        if (*rate > sdev_caps_stereo_maxrate) *rate = sdev_caps_stereo_maxrate;
    } else {
        if (*rate < 4000) *rate = 4000;
        if (*rate > sdev_caps_mono_maxrate) *rate = sdev_caps_mono_maxrate;
    }
    adjustRate(rate, *stereo, &tc);
}

void sbSetDSPTimeConst(const uint8_t tc) {
    sbioDSPWrite(sdev_hw_base, 0x40);
    sbioDSPWrite(sdev_hw_base, tc);
}

void sbSetDSPFrequency(const uint16_t freq) {
    sbioDSPWrite(sdev_hw_base, 0x41);
    sbioDSPWrite(sdev_hw_base, freq >> 8);
    sbioDSPWrite(sdev_hw_base, freq & 0xff);
}

void PUBLIC_CODE sbSetupMode(uint16_t freq, bool stereo) {
    uint8_t tc;

    sbioDSPReset(sdev_hw_base);

    /* Calculate time constant and adjust rate
       For SB PRO we have to setup double samplerate in stereo mode */
    adjustRate(&freq, stereo, &tc);

    /* Set DSP time constant or frequency */
    if (sbno == 6)
        sbSetDSPFrequency(freq);
    else
        sbSetDSPTimeConst(tc);

    /* Setup stereo option for SB PRO
       For SB16 it's set in DSP command */
    if (stereo & (sbno != 6))
        sbMixerWrite(0x0e, sbMixerRead(0x0e) || 0x02);

    /* Switch filter option off for SB PRO */
    if (sdev_caps_mixer)
        sbMixerWrite(0x0e, sbMixerRead(0x0e) || 0x20);

    setSpeaker(true);
}

void PUBLIC_CODE sbSetupDSPTransfer(uint16_t len, bool autoinit) {
    uint8_t cmd, mode;

    if (sbno == 6) {
        len--;
        if (sdev_mode_16bit) {
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
        if (sbno == 1) {
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
}

void PUBLIC_CODE sbSetupDMATransfer(void *p, uint16_t count, bool autoinit) {
    dmaMode_t mode;

    mode = DMA_MODE_TRAN_READ | DMA_MODE_ADDR_INCR | DMA_MODE_SINGLE;
    mode |= autoinit ? DMA_MODE_INIT_AUTO : DMA_MODE_INIT_SINGLE;

    if (! sdev_mode_16bit) {
        /* first the SBPRO stereo bugfix : */
        if (sdev_mode_stereo) {
            if (sbno < 6) {
                /* well ... should be a SB PRO in stereo mode ... */
                /* let's send one byte - nothing but silence */
                sbioDSPWrite(sdev_hw_base, 0x10);
                sbioDSPWrite(sdev_hw_base, 0x80);
            }
        }
    };

    dmaSetupSingleChannel(sdev_mode_16bit ? sdev_hw_dma16 : sdev_hw_dma8,
        mode, dmaGetLinearAddress(p), count);
}

uint16_t PUBLIC_CODE sbGetDMACounter(void) {
    return dmaGetCounter(sdev_mode_16bit ? sdev_hw_dma16 : sdev_hw_dma8);
}

void PUBLIC_CODE pause_play(void) {
    if (sdev_mode_16bit)
        sbioDSPWrite(sdev_hw_base, 0xd5);
    else
        sbioDSPWrite(sdev_hw_base, 0xd0);
}

void PUBLIC_CODE continue_play(void) {
    if (sdev_mode_16bit)
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

    setSpeaker(false);
}

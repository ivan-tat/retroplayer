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
#include "cc/dos.h"
#include "cc/errno.h"
#include "debug.h"
#include "hw/hwowner.h"
#include "hw/dma.h"
#include "hw/pic.h"
#include "hw/sndctl_t.h"
#include "hw/sb/sbio.h"

#include "hw/sb/sbctl.h"

#define toupper(c) (((c >= 'a') && (c <= 'z')) ? (c - 'a' + 'A') : c)

#define calc_time_const(r) (256 - 1000000UL / (r))
#define calc_time_const2(r) (256 - 500000UL / (r))
#define calc_rate(tc) (1000000UL / (256 - (tc)))
#define calc_rate2(tc) (500000UL / (256 - (tc)))

typedef uint8_t SBCAPS;

#define SBCAPS_MIXER    (1 << 0)    /* mixer chip is present */
#define SBCAPS_AUTOINIT (1 << 1)    /* autoinit is possible */
#define SBCAPS_STEREO   (1 << 2)    /* stereo play is possible */
#define SBCAPS_16BITS   (1 << 3)    /* 16-bits play is possible */

typedef uint8_t SBHWFLAGS;

#define SBHWFL_CONF     (1 << 0)    /* sound card is detected and properly configured */
#define SBHWFL_BASE     (1 << 1)    /* DSP base I/O address is detected */
#define SBHWFL_IRQ      (1 << 2)    /* IRQ channel is detected */
#define SBHWFL_DMA8     (1 << 3)    /* DMA 8-bits channel is detected */
#define SBHWFL_DMA16    (1 << 4)    /* DMA 16-bits channel is detected */

/*** DSP informational commands ***/

/* SB1 */
#define DSPC_GET_VERSION        0xe1

/* SB2 */
#define DSPC_GET_IDENTIFICATION 0xe0

/* SBPRO */
#define DSPC_GET_COPYRIGHT      0xe3

/*** DSP speaker commands ***/

/* SB1 */
#define DSPC_SPEAKER_ON         0xd1
#define DSPC_SPEAKER_OFF        0xd3

/*** DSP transfer control commands ***/

/* SB1 */
#define DSPC_SET_TIME_CONSTANT  0x40
#define DSPC_DMA8_DAC           0x14
#define DSPC_DMA8_ADC           0x24
#define DSPC_DMA8_HALT          0xd0
#define DSPC_DMA8_CONTINUE      0xd4

/* SB2 */
#define DSPC_SET_SIZE           0x48
#define DSPC_DMA8_DAC_AI        0x1c
#define DSPC_DMA8_ADC_AI        0x2c
#define DSPC_DMA8_DAC_HS        0x91
#define DSPC_DMA8_DAC_AI_HS     0x90
#define DSPC_DMA8_ADC_AI_HS     0x98
#define DSPC_DMA8_EXIT_AI       0xda

/* SB16 */
#define DSPC_SET_RATE           0x41
#define DSPC_DMA8_CONTINUE_AI   0x45
#define DSPC_DMA16_CONTINUE_AI  0x47
#define DSPC_DMA16_HALT         0xd5
#define DSPC_DMA16_CONTINUE     0xd6
#define DSPC_DMA16_EXIT_AI      0xd9

/*** Generic SB16 DSP transfer command (two bytes) ***/

/* SB16 generic DSP transfer command (1st byte) */
#define SB16_DSPC_MODE_MASK     0x02
#define SB16_DSPC_MODE_LIFO     0x00
#define SB16_DSPC_MODE_FIFO     0x02
#define SB16_DSPC_DMA_MASK      0x04
#define SB16_DSPC_DMA_SINGLE    0x00
#define SB16_DSPC_DMA_AUTOINIT  0x04
#define SB16_DSPC_DIR_MASK      0x08
#define SB16_DSPC_DIR_DAC       0x00
#define SB16_DSPC_DIR_ADC       0x08
#define SB16_DSPC_BITS_MASK     0xf0
#define SB16_DSPC_BITS_16       0xb0
#define SB16_DSPC_BITS_8        0xc0

/* SB16 generic DSP transfer mode (2nd byte) */
#define SB16_DSPM_SAMPLE_MASK       0x10
#define SB16_DSPM_SAMPLE_UNSIGNED   0x00
#define SB16_DSPM_SAMPLE_SIGNED     0x10
#define SB16_DSPM_CHANNELS_MASK     0x20
#define SB16_DSPM_CHANNELS_MONO     0x00
#define SB16_DSPM_CHANNELS_STEREO   0x20

/* transfer flags */

typedef uint8_t SBTRANSFERFLAGS;

#define SBTRFL_BUFFER   (1 << 0)
#define SBTRFL_MODE     (1 << 1)
#define SBTRFL_AUTOINIT (1 << 2)
#define SBTRFL_COMMANDS (1 << 3)
#define SBTRFL_ACTIVE   (1 << 4)

typedef struct sb_device_t
{
    SBMODEL   model;
    uint16_t  dspv;             // DSP chip version
    char     *name;
    SBCAPS    caps_flags;
    uint16_t  caps_rate_mono;   // max mono sample rate
    uint16_t  caps_rate_stereo; // max stereo sample rate
    SBHWFLAGS hw_flags;
    uint16_t  hw_base;          // DSP base I/O address
    uint8_t   hw_irq;           // IRQ channel
    uint8_t   hw_dma8;          // DMA channel for 8-bits play
    uint8_t   hw_dma16;         // DMA channel for 16-bits play
    SoundHWISRCallback_t *isr_user;
    uint8_t   irq_answer;       // for detecting
    SBTRANSFERFLAGS transfer_flags;
    // transfer buffer
    void     *transfer_buffer;
    uint16_t  transfer_frame_size;
    uint16_t  transfer_frames_count;
    // transfer mode
    uint16_t  transfer_mode_rate;
    uint8_t   transfer_mode_timeconst;
    bool      transfer_mode_stereo;
    bool      transfer_mode_16bits;
    bool      transfer_mode_signed;
    // transfer mode: DSP commands
    uint8_t   transfer_mode_DSP_start;  // SB1-SB16
    uint8_t   transfer_mode_DSP_mode;   // SB16 only
};

#define SELF ((struct sb_device_t *)self)

static const uint8_t _sb_silence_u8 = 0x80;
static const uint16_t _sb_silence_s16 = 0;

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

#define SBMODELS_MAX 4
static const struct
{
    SBMODEL model;
    char *name;
    char *comment;
    SBCAPS caps;
    uint16_t rate_mono;
    uint16_t rate_stereo;
} SBMODELS[SBMODELS_MAX] =
{
    {
        SBMODEL_SB1,
        "SoundBlaster 1.x",
        "24kHz mono @ 8 bits",
        0,
        calc_rate(calc_time_const(22050)),
        0
    },
    {
        SBMODEL_SB2,
        "SoundBlaster 2.x",
        "44kHz mono @ 8 bits",
        SBCAPS_MIXER | SBCAPS_AUTOINIT,
        calc_rate(calc_time_const(44100)),
        0
    },
    {
        SBMODEL_SBPRO,
        "SoundBlaster Pro/Pro 2",
        "44kHz mono / 22kHz stereo @ 8 bits",
        SBCAPS_MIXER | SBCAPS_AUTOINIT | SBCAPS_STEREO,
        calc_rate(calc_time_const(44100)),
        calc_rate2(calc_time_const2(22050))
    },
    {
        SBMODEL_SB16,
        "SoundBlaster 16/16 ASP",
        "45kHz mono/stereo @ 8/16 bits",
        SBCAPS_MIXER | SBCAPS_AUTOINIT | SBCAPS_STEREO | SBCAPS_16BITS,
        calc_rate(calc_time_const(44100)),
        calc_rate(calc_time_const(44100))
    }
};

/* Driver */

static HWOWNER _sbdriver;

/* ISR */

void __far _ISR_detect(SBDEV *self, uint8_t irq)
{
    _disable();

    sbioDSPAcknowledgeIRQ(SELF->hw_base, false);

    if (irq >= 8)
        pic_eoi(8);  /* secondary */

    pic_eoi(0);      /* primary */

    SELF->irq_answer = irq;

    _enable();
}

void __near _sb_start_DSP_transfer(SBDEV *self);

void __far _ISR_play(SBDEV *self, uint8_t irq)
{
    _disable();

    sbioDSPAcknowledgeIRQ(SELF->hw_base, SELF->transfer_mode_16bits);

    if (SELF->hw_irq >= 8)
        pic_eoi(8);  /* secondary */

    pic_eoi(0);  /* primary */

    if (SELF->isr_user)
        SELF->isr_user();

    if ((SELF->transfer_flags & SBTRFL_AUTOINIT) && !(SELF->caps_flags & SBCAPS_AUTOINIT))
        _sb_start_DSP_transfer(self);

    _enable();
}

/* Methods */

SBDEV *PUBLIC_CODE sb_new(void)
{
    uint16_t seg;

    if (!_dos_allocmem(_dos_para(sizeof(struct sb_device_t)), &seg))
        return MK_FP(seg, 0);
    else
        return NULL;
}

void PUBLIC_CODE sb_init(SBDEV *self)
{
    if (self)
    {
        memset(self, 0, sizeof(struct sb_device_t));
        /*
        _sb_unset_hw(self);
        _sb_unset_hw_flags(self);
        _sb_set_hw_config(self, 0x220, 7, 1, 5);
        _sb_unset_isr_user(self);
        _sb_unset_transfer_buffer(self);
        _sb_unset_transfer_mode(self);
        */
    }
}

uint16_t __near _sb_get_model_dspv(SBMODEL model)
{
    switch (model)
    {
    case SBMODEL_SB1:
        return 0x100;
    case SBMODEL_SB2:
        return 0x200;
    case SBMODEL_SBPRO:
        return 0x300;
    case SBMODEL_SB16:
        return 0x400;
    default:
        return 0;
    }
}

int __near _sb_find_model(SBMODEL model)
{
    int i;

    for (i = 0; i < SBMODELS_MAX; i++)
        if (SBMODELS[i].model == model)
            return i;

    return -1;
}

void __near _sb_set_hw(SBDEV *self, SBMODEL model, uint16_t dspv, char *name, SBCAPS flags, uint16_t rate_mono, uint16_t rate_stereo)
{
    SELF->model = model;
    SELF->dspv = dspv;
    SELF->name = name;
    SELF->caps_flags = flags;
    SELF->caps_rate_mono = rate_mono;
    SELF->caps_rate_stereo = rate_stereo;
}

void __near _sb_unset_hw(SBDEV *self)
{
    SELF->model = SBMODEL_UNKNOWN;
    SELF->dspv = 0;
    SELF->name = NULL;
    SELF->caps_flags = 0;
    SELF->caps_rate_mono = 0;
    SELF->caps_rate_stereo = 0;
}

void __near _sb_set_hw_dsp(SBDEV *self, uint16_t dspv)
{
    int i;

    switch (dspv >> 8)
    {
    case 1:
    case 2:
    case 3:
    case 4:
        i = (dspv >> 8) - 1;
       _sb_set_hw(
            self,
            SBMODELS[i].model,
            dspv,
            SBMODELS[i].name,
            SBMODELS[i].caps,
            SBMODELS[i].rate_mono,
            SBMODELS[i].rate_stereo
        );
        break;
    default:
        _sb_unset_hw(self);
        break;
    }
}

void __near _sb_set_hw_flags(SBDEV *self, SBHWFLAGS flags)
{
    SELF->hw_flags = flags;
}

void __near _sb_unset_hw_flags(SBDEV *self)
{
    SELF->hw_flags = 0;
}

void __near _sb_set_hw_config(SBDEV *self, uint16_t base, uint8_t irq, uint8_t dma8, uint8_t dma16)
{
    SELF->hw_base = base;
    SELF->hw_irq = irq;
    SELF->hw_dma8 = dma8;
    SELF->hw_dma16 = dma16;
}

void __near _sb_unset_hw_config(SBDEV *self)
{
    SELF->hw_base = -1;
    SELF->hw_irq = -1;
    SELF->hw_dma8 = -1;
    SELF->hw_dma16 = -1;
}

void __near _sb_set_isr_user(SBDEV *self, void *p)
{
    SELF->isr_user = p;
}

void __near _sb_unset_isr_user(SBDEV *self)
{
    SELF->isr_user = NULL;
}

void __near _sb_unset_transfer_mode_DSP_command(SBDEV *self)
{
    SELF->transfer_flags &= ~SBTRFL_COMMANDS;
    SELF->transfer_mode_DSP_start = 0;
    SELF->transfer_mode_DSP_mode = 0;
}

void __near _sb_unset_transfer_mode(SBDEV *self)
{
    SELF->transfer_flags &= ~SBTRFL_MODE;
    SELF->transfer_mode_rate = 0;
    SELF->transfer_mode_timeconst = 0;
    SELF->transfer_mode_stereo = false;
    SELF->transfer_mode_16bits = false;
    SELF->transfer_mode_signed = false;

    _sb_unset_transfer_mode_DSP_command(self);
}

void __near _sb_set_transfer_mode(SBDEV *self, bool f_mode, uint16_t m_rate, uint8_t m_timeconst, bool f_16bits, bool f_sign, bool f_stereo)
{
    if (f_mode)
        SELF->transfer_flags |= SBTRFL_MODE;
    else
        SELF->transfer_flags &= ~SBTRFL_MODE;

    SELF->transfer_mode_rate = m_rate;
    SELF->transfer_mode_timeconst = m_timeconst;
    SELF->transfer_mode_stereo = f_stereo;
    SELF->transfer_mode_16bits = f_16bits;
    SELF->transfer_mode_signed = f_sign;

    _sb_unset_transfer_mode_DSP_command(self);
}

void __near _sb_set_transfer_buffer(SBDEV *self, bool f_buffer, void *buffer, uint16_t frame_size, uint16_t frames_count, bool f_autoinit)
{
    if (f_buffer)
        SELF->transfer_flags |= SBTRFL_BUFFER;
    else
        SELF->transfer_flags &= ~SBTRFL_BUFFER;

    if (f_autoinit)
        SELF->transfer_flags |= SBTRFL_AUTOINIT;
    else
        SELF->transfer_flags &= ~SBTRFL_AUTOINIT;

    SELF->transfer_buffer = buffer;
    SELF->transfer_frame_size = frame_size;
    SELF->transfer_frames_count = frames_count;
}

void __near _sb_unset_transfer_buffer(SBDEV *self)
{
    SELF->transfer_flags &= ~(SBTRFL_BUFFER | SBTRFL_AUTOINIT);
    SELF->transfer_buffer = NULL;
    SELF->transfer_frame_size = 0;
    SELF->transfer_frames_count = 0;
}

/* This routine may not work for all registers because of different timings. */
void __near _sb_mixer_write(SBDEV *self, uint8_t reg, uint8_t data)
{
    if (SELF->caps_flags & SBCAPS_MIXER)
        sbioMixerWrite(SELF->hw_base, reg, data);
}

uint8_t __near _sb_mixer_read(SBDEV *self, uint8_t reg)
{
    if (SELF->caps_flags & SBCAPS_MIXER)
        return sbioMixerRead(SELF->hw_base, reg);
    else
        return 0;
}

char *PUBLIC_CODE sb_get_name(SBDEV *self)
{
    return SELF->name;
}

uint8_t PUBLIC_CODE sb_mode_get_bits(SBDEV *self)
{
    return SELF->transfer_mode_16bits ? 16 : 8;
}

bool PUBLIC_CODE sb_mode_is_signed(SBDEV *self)
{
    return SELF->transfer_mode_signed;
}

uint8_t PUBLIC_CODE sb_mode_get_channels(SBDEV *self)
{
    return SELF->transfer_mode_stereo ? 2 : 1;
}

uint16_t PUBLIC_CODE sb_mode_get_rate(SBDEV *self)
{
    return SELF->transfer_mode_rate;
}

void PUBLIC_CODE sb_hook_IRQ(SBDEV *self, void *p)
{
    DEBUG_BEGIN("sb_hook_IRQ");

    _sb_set_isr_user(self, p);
    hwowner_hook_irq(&_sbdriver, SELF->hw_irq, &_ISR_play, (void *)self);
    /* no changes for IRQ2 */
    pic_disable((1 << SELF->hw_irq) & ~(1 << 2));

    DEBUG_END("sb_hook_IRQ");
}

void PUBLIC_CODE sb_unhook_IRQ(SBDEV *self)
{
    DEBUG_BEGIN("sb_unhook_IRQ");

    /* no changes for IRQ2 */
    pic_enable((1 << SELF->hw_irq) & ~(1 << 2));
    hwowner_release_irq(&_sbdriver, SELF->hw_irq);

    DEBUG_END("sb_unhook_IRQ");
}

void PUBLIC_CODE sb_set_volume(SBDEV *self, uint8_t value)
{
    uint8_t b;

    if (SELF->caps_flags & SBCAPS_MIXER)
    {
        if (SELF->model == SBMODEL_SB16)
        {
            _sb_mixer_write(self, SBIO_MIXER_MASTER_LEFT, value);
            _sb_mixer_write(self, SBIO_MIXER_MASTER_RIGHT, value);
            _sb_mixer_write(self, SBIO_MIXER_VOICE_LEFT, value);
            _sb_mixer_write(self, SBIO_MIXER_VOICE_RIGHT, value);
        }
        else
        {
            if (value > 15)
                value = 15;
            value |= value << 4;
            _sb_mixer_write(self, SBIO_MIXER_MASTER_VOLUME, value);
            _sb_mixer_write(self, SBIO_MIXER_DAC_LEVEL, value);
        }
    }
}

void PUBLIC_CODE sb_set_transfer_buffer(SBDEV *self, void *buffer, uint16_t frame_size, uint16_t frames_count, bool autoinit)
{
    if (!(SELF->transfer_flags & SBTRFL_ACTIVE))
        _sb_set_transfer_buffer(self, true, buffer, frame_size, frames_count, autoinit);
}

uint16_t __near _sb_read_DSP_version(SBDEV *self)
{
    union
    {
        uint8_t lsb, msb;
        uint16_t w;
    } v;

    if (!sbioDSPWrite(SELF->hw_base, DSPC_GET_VERSION))
        return 0;

    v.msb = sbioDSPRead(SELF->hw_base);
    if (sbioError != E_SBIO_SUCCESS)
        return 0;

    v.lsb = sbioDSPRead(SELF->hw_base);
    if (sbioError != E_SBIO_SUCCESS)
        return 0;

    return v.w;
}

void __near _sb_set_speaker(SBDEV *self, bool state)
{
    uint8_t cmd;
    uint16_t wait;

    if (state)
    {
        cmd = DSPC_SPEAKER_ON;
        wait = 110;
    }
    else
    {
        cmd = DSPC_SPEAKER_OFF;
        wait = 220;
    }

    if (sbioDSPWrite(SELF->hw_base, cmd))
        delay(wait);
}

void __near _sb_adjust_rate(SBDEV *self, uint16_t *rate, bool stereo, uint8_t *tc)
{
    uint8_t timeconst;

    if (stereo)
    {
        if (*rate < 4000)
            *rate = 4000;
        else
        if (*rate > SELF->caps_rate_stereo)
            *rate = SELF->caps_rate_stereo;
    }
    else
    {
        if (*rate < 4000)
            *rate = 4000;
        else
        if (*rate > SELF->caps_rate_mono)
            *rate = SELF->caps_rate_mono;
    }

    if ((SELF->model == SBMODEL_SB16) || !stereo)
    {
        *tc = calc_time_const(*rate);
        *rate = calc_rate(*tc);
    }
    else
    {
        *tc = calc_time_const2(*rate);
        *rate = calc_rate2(*tc);
    }
}

void __near _sb_adjust_transfer_mode(SBDEV *self, uint16_t *m_rate, uint8_t *m_tc, uint8_t *m_channels, uint8_t *m_bits, bool *f_sign)
{
    bool m_stereo;
    bool m_16bits;

    m_stereo = (*m_channels == 2) && (SELF->caps_flags & SBCAPS_STEREO);
    m_16bits = (*m_bits == 16) && (SELF->caps_flags & SBCAPS_16BITS);
    *f_sign = *f_sign && (SELF->caps_flags & SBCAPS_16BITS); // adjust f_sign

    _sb_adjust_rate(self, m_rate, m_stereo, m_tc);  // adjust m_rate

    *m_channels = m_stereo ? 2 : 1; // adjust m_channels
    *m_bits = m_16bits ? 16 : 8;    // adjust m_bits
}

void PUBLIC_CODE sb_adjust_transfer_mode(SBDEV *self, uint16_t *m_rate, uint8_t *m_channels, uint8_t *m_bits, bool *f_sign)
{
    uint8_t m_timeconst;

    DEBUG_BEGIN("sb_adjust_transfer_mode");

    if (SELF->model != SBMODEL_UNKNOWN)
    {
        DEBUG_INFO_(
            "sb_adjust_transfer_mode",
            "(in) rate=%u, channels=%hu, bits=%hu, sign=%c.",
            *m_rate,
            *m_channels,
            *m_bits,
            *f_sign ? 'Y' : 'N'
        );

        _sb_adjust_transfer_mode(self, m_rate, &m_timeconst, m_channels, m_bits, f_sign);

        DEBUG_INFO_(
            "sb_adjust_transfer_mode",
            "(out) rate=%u, channels=%hu, bits=%hu, sign=%c.",
            *m_rate,
            *m_channels,
            *m_bits,
            *f_sign ? 'Y' : 'N'
        );

        DEBUG_SUCCESS("sb_adjust_transfer_mode");
    }
    else
        DEBUG_FAIL("sb_adjust_transfer_mode", "No sound device.");
}

void PUBLIC_CODE sb_set_transfer_mode(SBDEV *self, uint16_t m_rate, uint8_t m_channels, uint8_t m_bits, bool f_sign)
{
    uint8_t m_timeconst;

    DEBUG_BEGIN("sb_set_transfer_mode");

    if (SELF->model != SBMODEL_UNKNOWN)
    {
        _sb_adjust_transfer_mode(self, &m_rate, &m_timeconst, &m_channels, &m_bits, &f_sign);

        SELF->transfer_flags |= SBTRFL_MODE;
        SELF->transfer_mode_rate = m_rate;
        SELF->transfer_mode_timeconst = m_timeconst;
        SELF->transfer_mode_stereo = m_channels == 2;
        SELF->transfer_mode_16bits = m_bits == 16;
        SELF->transfer_mode_signed = f_sign;

        DEBUG_SUCCESS("sb_set_transfer_mode");
    }
    else
        DEBUG_FAIL("sb_set_transfer_mode", "No sound device.");

}

void __near _sb_set_DSP_time_constant(SBDEV *self, const uint8_t tc)
{
    uint8_t data[2];

    data[0] = DSPC_SET_TIME_CONSTANT;
    data[1] = tc;
    sbioDSPWriteQueue(SELF->hw_base, &data, 2);
}

void __near _sb_set_DSP_rate(SBDEV *self, const uint16_t rate)
{
    uint8_t data[3];

    data[0] = DSPC_SET_RATE;
    data[1] = rate >> 8;
    data[2] = rate & 0xff;
    sbioDSPWriteQueue(SELF->hw_base, &data, 3);
}

void __near _sb_setup_transfer_mode_DSP_commands(SBDEV *self)
{
    uint8_t cmd, mode;
    uint16_t samplerate, midrate;

    switch (SELF->model)
    {
    case SBMODEL_SB1:
        cmd = DSPC_DMA8_DAC;
        mode = 0;
        break;
    case SBMODEL_SB2:
    case SBMODEL_SBPRO:
        samplerate = SELF->transfer_mode_rate * (SELF->transfer_mode_stereo ? 2 : 1);

        midrate = SELF->transfer_mode_stereo ?
            calc_rate2(calc_time_const2(22050)) : calc_rate(calc_time_const(22050));

        if (samplerate < midrate)
            cmd = (SELF->transfer_flags & SBTRFL_AUTOINIT) ? DSPC_DMA8_DAC_AI : DSPC_DMA8_DAC;
        else
            cmd = (SELF->transfer_flags & SBTRFL_AUTOINIT) ? DSPC_DMA8_DAC_AI_HS : DSPC_DMA8_DAC_HS;

        mode = 0;
        break;
    case SBMODEL_SB16:
        cmd = SB16_DSPC_MODE_FIFO | SB16_DSPC_DIR_DAC;
        cmd |= (SELF->transfer_flags & SBTRFL_AUTOINIT) ? SB16_DSPC_DMA_AUTOINIT : SB16_DSPC_DMA_SINGLE;
        cmd |= SELF->transfer_mode_16bits ? SB16_DSPC_BITS_16 : SB16_DSPC_BITS_8;
        mode = SELF->transfer_mode_signed ? SB16_DSPM_SAMPLE_SIGNED : SB16_DSPM_SAMPLE_UNSIGNED;
        mode |= SELF->transfer_mode_stereo ? SB16_DSPM_CHANNELS_STEREO : SB16_DSPM_CHANNELS_MONO;
        break;
    default:
        DEBUG_FAIL("_sb_setup_transfer_mode_DSP_commands", "Unknown sound device.");
        return;
    }

    SELF->transfer_mode_DSP_start = cmd;
    SELF->transfer_mode_DSP_mode = mode;

    SELF->transfer_flags |= SBTRFL_COMMANDS;
}

void __near _sb_start_DSP_transfer(SBDEV *self)
{
    uint16_t frame_size;
    uint8_t data[4];
    uint16_t length;

    frame_size = SELF->transfer_frame_size - 1;

    if (!(SELF->transfer_flags & SBTRFL_COMMANDS))
        _sb_setup_transfer_mode_DSP_commands(self);

    switch (SELF->model)
    {
    case SBMODEL_SB1:
        data[0] = SELF->transfer_mode_DSP_start;
        data[1] = frame_size & 0xff;
        data[2] = frame_size >> 8;
        length = 3;
        break;
    case SBMODEL_SB2:
    case SBMODEL_SBPRO:
        data[0] = DSPC_SET_SIZE;
        data[1] = frame_size & 0xff;
        data[2] = frame_size >> 8;
        data[3] = SELF->transfer_mode_DSP_start;
        length = 4;
        break;
    case SBMODEL_SB16:
        data[0] = SELF->transfer_mode_DSP_start;
        data[1] = SELF->transfer_mode_DSP_mode;
        data[2] = frame_size & 0xff;
        data[3] = frame_size >> 8;
        length = 4;
        break;
    default:
        DEBUG_FAIL("_sb_start_DSP_transfer", "Unknown sound device.");
        length = 0;
        break;
    }

    if (length)
        sbioDSPWriteQueue(SELF->hw_base, &data, length);
}

// count: number of bytes (for 8-bits channel) or number of words (for 16-bits channel)
void __near _sb_start_DMA_transfer(SBDEV *self)
{
    uint32_t count;
    dmaMode_t mode;

    count = SELF->transfer_frame_size * SELF->transfer_frames_count;

    if (SELF->transfer_mode_16bits)
        count /= 2;

    mode = DMA_MODE_TRAN_READ | DMA_MODE_ADDR_INCR | DMA_MODE_SINGLE;
    mode |= (SELF->transfer_flags & SBTRFL_AUTOINIT) ? DMA_MODE_INIT_AUTO : DMA_MODE_INIT_SINGLE;

    dmaSetupSingleChannel(
        SELF->transfer_mode_16bits ? SELF->hw_dma16 : SELF->hw_dma8,
        mode,
        dmaGetLinearAddress(SELF->transfer_buffer),
        count
    );
}

bool PUBLIC_CODE sb_transfer_start(SBDEV *self)
{
    uint8_t v;

    DEBUG_BEGIN("sb_transfer_start");

    if (!(SELF->transfer_flags & SBTRFL_BUFFER))
    {
        DEBUG_FAIL("sb_transfer_start", "Transfer buffer is not set.");
        return false;
    }

    if (!(SELF->transfer_flags & SBTRFL_MODE))
    {
        DEBUG_FAIL("sb_transfer_start", "Transfer mode is not set.");
        return false;
    }

    sbioDSPAcknowledgeIRQ(SELF->hw_base, false);
    sbioDSPAcknowledgeIRQ(SELF->hw_base, true);
    sb_transfer_stop(self);

    if (SELF->model == SBMODEL_SB16)
        _sb_set_DSP_rate(self, SELF->transfer_mode_rate);
    else
    {
        _sb_set_DSP_time_constant(self, SELF->transfer_mode_timeconst);

        if (SELF->model == SBMODEL_SBPRO)
        {
            v = _sb_mixer_read(self, 0x0e) | 0x20;   /* turn filter 'off' */

            if (SELF->transfer_mode_stereo)
                v |= 0x02;   /* turn stereo 'on' */

            _sb_mixer_write(self, 0x0e, v);
        }
    }

    _sb_set_speaker(self, true);

    _sb_start_DMA_transfer(self);
    _sb_start_DSP_transfer(self);

    SELF->transfer_flags |= SBTRFL_ACTIVE;

    DEBUG_SUCCESS("sb_transfer_start");
    return true;
}

uint16_t PUBLIC_CODE sb_get_DMA_counter(SBDEV *self)
{
    if (SELF->transfer_flags & SBTRFL_ACTIVE)
        return dmaGetCounter(SELF->transfer_mode_16bits ? SELF->hw_dma16 : SELF->hw_dma8);
    else
        return 0;
}

void __near _sb_transfer_stop_8(SBDEV *self)
{
    uint8_t data[3];
    uint16_t length;

    switch (SELF->model)
    {
    case SBMODEL_SB1:
        data[0] = DSPC_DMA8_HALT;
        length = 1;
        break;
    case SBMODEL_SB2:
    case SBMODEL_SBPRO:
    case SBMODEL_SB16:
        data[0] = DSPC_DMA8_HALT;
        data[1] = DSPC_DMA8_EXIT_AI;
        data[2] = DSPC_DMA8_HALT;
        length = 3;
        break;
    default:
        length = 0;
        break;
    }

    if (length)
    {
        sbioDSPWriteQueue(SELF->hw_base, &data, length);
        SELF->transfer_flags &= ~SBTRFL_ACTIVE;
    }
}

void __near _sb_transfer_stop_16(SBDEV *self)
{
    uint8_t data[3];
    uint16_t length;

    length = 0;

    switch (SELF->model)
    {
    case SBMODEL_SB1:
    case SBMODEL_SB2:
    case SBMODEL_SBPRO:
        break;
    case SBMODEL_SB16:
        data[0] = DSPC_DMA16_HALT;
        data[1] = DSPC_DMA16_EXIT_AI;
        data[2] = DSPC_DMA16_HALT;
        length += 3;
        break;
    default:
        break;
    }

    if (length)
    {
        sbioDSPWriteQueue(SELF->hw_base, &data, length);
        SELF->transfer_flags &= ~SBTRFL_ACTIVE;
    }
}

void PUBLIC_CODE sb_transfer_pause(SBDEV *self)
{
    if (SELF->transfer_flags & SBTRFL_ACTIVE)
        sbioDSPWrite(SELF->hw_base,
            SELF->transfer_mode_16bits ? DSPC_DMA16_HALT : DSPC_DMA8_HALT);
}

void PUBLIC_CODE sb_transfer_continue(SBDEV *self)
{
    if (SELF->transfer_flags & SBTRFL_ACTIVE)
        sbioDSPWrite(SELF->hw_base,
            SELF->transfer_mode_16bits ? DSPC_DMA16_CONTINUE : DSPC_DMA8_CONTINUE);
}

void PUBLIC_CODE sb_transfer_stop(SBDEV *self)
{
    uint8_t count;
    uint8_t chn;
    uint8_t mask;

    _sb_transfer_stop_8(self);
    _sb_transfer_stop_16(self);

    /* reset is the best way to make sure SB stops playing */
    sbioDSPReset(SELF->hw_base);

    count = 0;
    chn = -1;
    mask = 0;

    if (SELF->hw_flags & SBHWFL_DMA8)
    {
        count++;
        chn = SELF->hw_dma8;
        mask |= 1 << SELF->hw_dma8;
    }

    if (SELF->hw_flags & SBHWFL_DMA16)
    {
        count++;
        chn = SELF->hw_dma16;
        mask |= 1 << SELF->hw_dma16;
    }

    if ((count == 1) || (SELF->hw_dma8 == SELF->hw_dma16))
        dmaMaskSingleChannel(chn);
    else
    if (count > 1)
        dmaMaskChannels(mask);

    _sb_set_speaker(self, false);
}

bool __near _sb_detect_DSP_base(SBDEV *self)
{
    uint16_t p;
    uint16_t i;

    DEBUG_BEGIN("_sb_detect_DSP_base");

    if (SELF->hw_flags & SBHWFL_BASE)
    {
        DEBUG_SUCCESS("_sb_detect_DSP_base");
        return true;
    }

    i = 0;
    while ((!SELF->hw_flags & SBHWFL_BASE) && (i < HW_BASE_MAX))
    {
        p = HW_BASE_NUM[i];
        DEBUG_MSG_("_sb_detect_DSP_base", " - probing port 0x%04X...", p);

        if (sbioDSPReset(p))
        {
            SELF->hw_base = p;
            SELF->hw_flags |= SBHWFL_BASE;
        }

        i++;
    }

    if (!(SELF->hw_flags & SBHWFL_BASE))
    {
        DEBUG_FAIL("_sb_detect_DSP_base", "DSP not found.");
        return false;
    }

    #ifdef DEBUG
    if (SELF->hw_flags & SBHWFL_BASE)
        DEBUG_INFO_("_sb_detect_DSP_base", "Found DSP at base port 0x04X.", p);
    #endif

    SELF->dspv = _sb_read_DSP_version(self);
    if (sbioError != E_SBIO_SUCCESS)
    {
        DEBUG_FAIL("_sb_detect_DSP_base", "Unable to get DSP chip version.");
        return false;
    }

    if (((SELF->dspv >> 8) < 1) || ((SELF->dspv >> 8) > 4))
    {
        DEBUG_FAIL("_sb_detect_DSP_base", "Unknown DSP chip version.");
        return false;
    }

    DEBUG_SUCCESS("_sb_detect_DSP_base");
    return true;
}

bool __near _sb_detect_IRQ(SBDEV *self, uint8_t dma, bool f_16bits)
{
    DEBUG_BEGIN("_sb_detect_IRQ");

    SELF->irq_answer = 0;

    if (f_16bits)
        SELF->hw_dma16 = dma;
    else
        SELF->hw_dma8 = dma;

    sb_transfer_stop(self);
    sb_set_transfer_buffer(self, (void *)&_sb_silence_u8, 1, 1, false);
    sb_set_transfer_mode(self, 8000, 1, 8, false);
    sb_transfer_start(self);
    delay(10);

    if (SELF->irq_answer)
    {
        #ifdef DEBUG
        DEBUG_INFO_("_sb_detect_IRQ", "Found IRQ channel %hu.", SELF->irq_answer);
        #endif
        DEBUG_SUCCESS("_sb_detect_IRQ");

        if (f_16bits)
            SELF->hw_flags |= SBHWFL_DMA16;
        else
            SELF->hw_flags |= SBHWFL_DMA8;

        SELF->hw_irq = SELF->irq_answer;
        SELF->hw_flags |= SBHWFL_IRQ;
        return true;
    }
    else
    {
        DEBUG_FAIL("_sb_detect_IRQ", "No DMA and IRQ channels were found.");
        return false;
    }
}

bool __near _sb_detect_DMA_IRQ(SBDEV *self)
{
    void *oldv[HW_IRQ_MAX];
    uint8_t i;
    uint8_t dmac;
    uint8_t dmamask;
    IRQMASK irqmask;

    DEBUG_BEGIN("_sb_detect_DMA_IRQ");

    if (SELF->hw_flags & SBHWFL_DMA8)
    {
        DEBUG_SUCCESS("_sb_detect_DMA_IRQ");
        return true;
    }
    if (!(SELF->hw_flags & SBHWFL_BASE))
    {
        DEBUG_FAIL("_sb_detect_DMA_IRQ", "DSP base port is not set.");
        return false;
    }

    dmamask = 0;
    for (i = 0; i < HW_DMA_MAX; i++)
        dmamask |= 1 << HW_DMA_NUM[i];

    dmaMaskChannels(dmamask);

    _enable();

    irqmask = 0;
    for (i = 0; i < HW_IRQ_MAX; i++)
        irqmask |= 1 << HW_IRQ_NUM[i];

    irqmask &= ~pic_get_hooked_irq_channels();
    if (!irqmask)
    {
        DEBUG_FAIL("_sb_detect_DMA_IRQ", "No free IRQ channels are available.");
        return false;
    }

    if (!hwowner_hook_irq_channels(&_sbdriver, irqmask, &_ISR_detect, self))
    {
        DEBUG_FAIL("_sb_detect_DMA_IRQ", "Failed to hook IRQ channels.");
        return false;
    }
    /* no changes for IRQ 2 */
    pic_disable(irqmask & ~(1 << 2));

    i = 0;
    while ((!(SELF->hw_flags & SBHWFL_DMA8)) && (i < HW_DMA_MAX))
    {
        dmac = HW_DMA_NUM[i];

        DEBUG_MSG_("_sb_detect_DMA_IRQ", "- trying DMA channel %hu...", dmac);

        _sb_detect_IRQ(self, dmac, false);

        i++;
    }

    if (!hwowner_release_irq_channels(&_sbdriver, irqmask))
    {
        DEBUG_FAIL("_sb_detect_DMA_IRQ", "Failed to release IRQ channels.");
        return false;
    }

    /* no changes for IRQ 2 */
    pic_enable(irqmask & ~(1 << 2));

    if (!(SELF->hw_flags & SBHWFL_DMA8))
    {
        DEBUG_FAIL("_sb_detect_DMA_IRQ", "DMA and IRQ channels were not found.");
        return false;
    }

    sbioDSPReset(SELF->hw_base);

    DEBUG_SUCCESS("_sb_detect_DMA_IRQ");
    return true;
}

bool PUBLIC_CODE sb_conf_detect(SBDEV *self)
{
    DEBUG_BEGIN("sb_conf_detect");

    _sb_unset_hw_flags(self);
    _sb_unset_hw(self);
    _sb_set_transfer_mode(self, true, 8000, 1, calc_time_const(8000), 8, false);

    if (!_sb_detect_DSP_base(self))
    {
        DEBUG_FAIL("sb_conf_detect", "No DSP base I/O address specified.");
        return false;
    }

    /* for the first set SB1.0 - should work on all SBs */
    _sb_set_hw_dsp(self, _sb_get_model_dspv(SBMODEL_SB1));

    sb_transfer_stop(self);

    if (!_sb_detect_DMA_IRQ(self))
    {
        DEBUG_FAIL("sb_conf_detect", "Failed to find DMA and IRQ channels.");
        SELF->model = SBMODEL_UNKNOWN;
        return false;
    }

    sbioDSPReset(SELF->hw_base);

    _sb_set_hw_dsp(self, SELF->dspv);

    if (SELF->model != SBMODEL_UNKNOWN)
    {
        DEBUG_SUCCESS("sb_conf_detect");
        return true;
    }
    else
    {
        DEBUG_FAIL("sb_conf_detect", "Unable to detect SoundBlaster.");
        return false;
    }
}

void PUBLIC_CODE sb_conf_manual(SBDEV *self, SBCFGFLAGS flags, SBMODEL model, uint16_t base, uint8_t irq, uint8_t dma8, uint8_t dma16)
{
    uint16_t dspv;
    SBHWFLAGS hwflags;

    if (flags & SBCFGFL_TYPE)
        dspv = _sb_get_model_dspv(model);
    else
        dspv = 0;

    if (dspv)
    {
        hwflags = 0;
        if (flags & SBCFGFL_BASE)
            hwflags |= SBHWFL_BASE;
        if (flags & SBCFGFL_IRQ)
            hwflags |= SBHWFL_IRQ;
        if (flags & SBCFGFL_DMA8)
            hwflags |= SBHWFL_DMA8;
        if (flags & SBCFGFL_DMA16)
            hwflags |= SBHWFL_DMA16;

        _sb_set_hw_dsp(self, dspv);
        _sb_set_hw_flags(self, hwflags);
        _sb_set_hw_config(self, base, irq, dma8, dma16);
    }
    else
        _sb_unset_hw(self);

    _sb_unset_transfer_buffer(self);
    _sb_unset_transfer_mode(self);
}

bool __near _check_value_type(long int v)
{
    return (v >= 1) && (v <= 6);
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

bool __near _select_model(SBMODEL *v)
{
    char c;

    printf(CRLF
        "Select type:" CRLF
        "0) none " CRLF
    );
    for (c = 0; c < SBMODELS_MAX; c++)
        printf(
            "%c) %s (%s)" CRLF,
            '1' + c, SBMODELS[c].name, SBMODELS[c].comment
        );

    do
    {
        c = getch();
    } while (!(c >= '0' && c <= '0' + SBMODELS_MAX));

    c -= '0';

    if (!c || c > SBMODELS_MAX)
        return false;

    *v = SBMODELS[c - 1].model;

    printf("Selected model = %s." CRLF, SBMODELS[c - 1].name);
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

bool PUBLIC_CODE sb_conf_input(SBDEV *self)
{
    SBCFGFLAGS flags;
    SBMODEL model;
    uint16_t base;
    uint8_t irq, dma8, dma16;

    DEBUG_BEGIN("sb_conf_input");

    if (!_select_model(&model))
    {
        DEBUG_FAIL("sb_conf_input", "Cancelled");
        return false;
    }

    if (!_select_DSP(&base))
    {
        DEBUG_FAIL("sb_conf_input", "Cancelled");
        return false;
    }

    if (!_select_IRQ(&irq))
    {
        DEBUG_FAIL("sb_conf_input", "Cancelled");
        return false;
    }

    if (!_select_DMA(&dma8, 8))
    {
        DEBUG_FAIL("sb_conf_input", "Cancelled");
        return false;
    }

    flags = SBCFGFL_TYPE | SBCFGFL_BASE | SBCFGFL_IRQ | SBCFGFL_DMA8;

    if (model == SBMODEL_SB16)
    {
        if (!_select_DMA(&dma16, 16))
        {
            DEBUG_FAIL("sb_conf_input", "Cancelled");
            return false;
        }
        flags |= SBCFGFL_DMA16;
    }
    else
        dma16 = -1;

    sb_conf_manual(self, flags, model, base, irq, dma8, dma16);

    DEBUG_SUCCESS("sb_conf_input");
    return true;
}

bool PUBLIC_CODE sb_conf_env(SBDEV *self)
{
    char s[256], *param, *endptr;
    uint8_t type;
    uint16_t base;
    uint8_t irq, dma8, dma16;
    uint16_t len, i;
    SBCFGFLAGS flags;
    long int v;
    SBMODEL model;

    DEBUG_BEGIN("sb_conf_env");

    _sb_unset_hw(self);
    _sb_unset_hw_flags(self);
    _sb_unset_hw_config(self);
    _sb_unset_transfer_buffer(self);
    _sb_unset_transfer_mode(self);

    custom_getenv(s, "BLASTER", 255);
    len = strlen(s);
    if (!len)
    {
        DEBUG_FAIL("sb_conf_env", "BLASTER environment variable is not set.");
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
            DEBUG_INFO_("sb_conf_env", "Type=%u", (uint16_t)v);
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
            DEBUG_INFO_("sb_conf_env", "BASE=0x%04X", (uint16_t)v);
            base = v;
            flags |= SBCFGFL_BASE;
        }
    }

    param = strchr(s, 'I');
    if (param)
    {
        errno = 0;
        v = strtol(param + 1, &endptr, 10);
        if ((!errno) && _check_value_irq(v))
        {
            DEBUG_INFO_("sb_conf_env", "IRQ=%hu", (uint8_t)v);
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
            DEBUG_INFO_("sb_conf_env", "DMA8=%hu", (uint8_t)v);
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
            DEBUG_INFO_("sb_conf_env", "DMA16=%hu", (uint8_t)v);
            dma16 = v;
            flags |= SBCFGFL_DMA16;
        }
    }

    if (flags & SBCFGFL_BASE_MASK == SBCFGFL_BASE_MASK)
    {
        switch (type)
        {
        case 1:
        case 2:
            model = SBMODEL_SB1;
            break;
        case 3:
            model = SBMODEL_SB2;
            break;
        case 4:
        case 5:
            model = SBMODEL_SBPRO;
            break;
        case 6:
            model = SBMODEL_SB16;
            break;
        default:
            model = SBMODEL_UNKNOWN;
            flags &= ~SBCFGFL_TYPE;
            break;
        }
        sb_conf_manual(self, flags, model, base, irq, dma8, dma16);
    }
    else
    {
        DEBUG_FAIL("sb_conf_env", "Configuration string is not complete.");
        return false;
    }

    DEBUG_SUCCESS("sb_conf_env");
    return true;
}

void PUBLIC_CODE sb_conf_dump(SBDEV *self)
{
    int i;

    printf("Sound device: " CRLF);
    i = _sb_find_model(SELF->model);
    if (i >= 0)
        printf("%s (%s).", SBMODELS[i].name, SBMODELS[i].comment);
    else
        printf("N/A." CRLF);

    printf("Hardware DSP base I/O address: ");
    if (SELF->hw_flags & SBHWFL_BASE)
        printf("0x%04X." CRLF, SELF->hw_base);
    else
        printf("N/A." CRLF);

    printf("Hardware IRQ channel: ");
    if (SELF->hw_flags & SBHWFL_IRQ)
        printf("%hu." CRLF, SELF->hw_irq);
    else
        printf("N/A." CRLF);

    printf("Hardware 8-bits DMA channel: ");
    if (SELF->hw_flags & SBHWFL_DMA8)
        printf("%hu." CRLF, SELF->hw_dma8);
    else
        printf("N/A." CRLF);

    printf("Hardware 16-bits DMA channel: ");
    if (SELF->hw_flags & SBHWFL_DMA16)
        printf("%hu." CRLF, SELF->hw_dma16);
    else
        printf("N/A." CRLF);
}

void PUBLIC_CODE sb_delete(SBDEV **self)
{
    if (self)
        if (*self)
        {
            _dos_freemem(FP_SEG(*self));
            *self = NULL;
        }
}

/*** Initialization ***/

void sbctl_init(void)
{
    DEBUG_BEGIN("sbctl_init");

    hwowner_init(&_sbdriver, "Internal SoundBlaster driver");

    DEBUG_END("sbctl_init");
}

void sbctl_done(void)
{
    DEBUG_BEGIN("sbctl_done");

    hwowner_free(&_sbdriver);

    DEBUG_END("sbctl_done");
}

DEFINE_REGISTRATION(sbctl, sbctl_init, sbctl_done)

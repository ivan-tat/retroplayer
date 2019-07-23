/* sbctl.c -- Sound Blaster hardware control library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$hw$sb$sbctl$*"
#endif

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

typedef uint16_t SBHWFLAGS;

#define SBHWFL_CONF         (1 << 0)   /* sound card is detected and properly configured */
#define SBHWFL_BASE         (1 << 1)   /* DSP base I/O address is detected */
#define SBHWFL_IRQ          (1 << 2)   /* IRQ channel is detected */
#define SBHWFL_DMA8         (1 << 3)   /* DMA 8-bits channel is detected */
#define SBHWFL_DMA16        (1 << 4)   /* DMA 16-bits channel is detected */
#define SBHWFL_IRQ_HOOKED   (1 << 5)   /* IRQ channel is hooked */
#define SBHWFL_DMA8_HOOKED  (1 << 6)   /* DMA 8-bits channel is hooked */
#define SBHWFL_DMA16_HOOKED (1 << 7)   /* DMA 16-bits channel is hooked */

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

#define SBTRFL_MODE     (1 << 0)
#define SBTRFL_BUFFER   (1 << 1)
#define SBTRFL_AUTOINIT (1 << 2)
#define SBTRFL_COMMANDS (1 << 3)
#define SBTRFL_ACTIVE   (1 << 4)

/* transfer mode flags */

typedef uint8_t SBMODEFLAGS;

#define SBMODEFL_SIGNED (1 << 0)

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
    SBTRANSFERFLAGS transfer_flags;
    // transfer buffer
    void     *transfer_buffer;
    uint16_t  transfer_frame_size;
    uint16_t  transfer_frames_count;
    SoundHWISRCallback_t *transfer_callback;
    void     *transfer_callback_param;
    // transfer mode
    SBMODEFLAGS transfer_mode_flags;
    uint16_t  transfer_mode_rate;
    uint8_t   transfer_mode_timeconst;
    uint8_t   transfer_mode_channels;
    uint8_t   transfer_mode_bits;
    // transfer mode: DSP commands
    uint8_t   transfer_mode_DSP_start;  // SB1-SB16
    uint8_t   transfer_mode_DSP_mode;   // SB16 only
};

// for buggy Open Watcom compiler:
#define declare_Self struct sb_device_t *_Self = self

static const uint16_t _sb_silence_u = 0x8080;

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
        "22kHz mono @ 8 bits",
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
        "44kHz mono/stereo @ 8/16 bits",
        SBCAPS_MIXER | SBCAPS_AUTOINIT | SBCAPS_STEREO | SBCAPS_16BITS,
        calc_rate(calc_time_const(44100)),
        calc_rate(calc_time_const(44100))
    }
};

/* Driver */

static HWOWNER *_sbdriver;

/* ISR */

void __far _ISR_detect(SBDEV *self, uint8_t irq)
{
    declare_Self;

    _disable();

    sbioDSPAcknowledgeIRQ(_Self->hw_base, _Self->transfer_mode_bits == 16);

    hwowner_eoi(_sbdriver, irq);

    _Self->hw_irq = irq;

    _enable();
}

bool __near _sb_start_DSP_transfer(SBDEV *self);

void __far _ISR_play(SBDEV *self, uint8_t irq)
{
    declare_Self;

    _disable();

    sbioDSPAcknowledgeIRQ(_Self->hw_base, _Self->transfer_mode_bits == 16);

    hwowner_eoi(_sbdriver, irq);

    _enable();

    if (_Self->transfer_callback)
        _Self->transfer_callback (_Self->transfer_callback_param);

    if ((_Self->transfer_flags & SBTRFL_AUTOINIT) && !(_Self->caps_flags & SBCAPS_AUTOINIT))
        _sb_start_DSP_transfer(_Self);
}

/* Private methods, assuming 'self != NULL' */

/* This routine may not work for all registers because of different timings. */
void __near _sb_mixer_write(SBDEV *self, uint8_t reg, uint8_t data)
{
    declare_Self;

    if (_Self->caps_flags & SBCAPS_MIXER)
        sbioMixerWrite(_Self->hw_base, reg, data);
}

uint8_t __near _sb_mixer_read(SBDEV *self, uint8_t reg)
{
    declare_Self;

    if (_Self->caps_flags & SBCAPS_MIXER)
        return sbioMixerRead(_Self->hw_base, reg);
    else
        return 0;
}

uint16_t __near _sb_read_DSP_version(SBDEV *self)
{
    declare_Self;
    union
    {
        uint8_t lsb, msb;
        uint16_t w;
    } v;

    if (!sbioDSPWrite(_Self->hw_base, DSPC_GET_VERSION))
        return 0;

    v.msb = sbioDSPRead(_Self->hw_base);
    if (sbioError != E_SBIO_SUCCESS)
        return 0;

    v.lsb = sbioDSPRead(_Self->hw_base);
    if (sbioError != E_SBIO_SUCCESS)
        return 0;

    return v.w;
}

bool __near _sb_set_DSP_time_constant(SBDEV *self, const uint8_t tc)
{
    declare_Self;
    uint8_t data[2];

    data[0] = DSPC_SET_TIME_CONSTANT;
    data[1] = tc;
    return sbioDSPWriteQueue(_Self->hw_base, &data, 2);
}

bool __near _sb_set_DSP_rate(SBDEV *self, const uint16_t rate)
{
    declare_Self;
    uint8_t data[3];

    data[0] = DSPC_SET_RATE;
    data[1] = rate >> 8;
    data[2] = rate & 0xff;
    return sbioDSPWriteQueue(_Self->hw_base, &data, 3);
}

bool __near _sb_set_speaker(SBDEV *self, bool state)
{
    declare_Self;
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

    if (!sbioDSPWrite(_Self->hw_base, cmd))
        return false;

    delay(wait);
    return true;
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
    declare_Self;

    _Self->model = model;
    _Self->dspv = dspv;
    _Self->name = name;
    _Self->caps_flags = flags;
    _Self->caps_rate_mono = rate_mono;
    _Self->caps_rate_stereo = rate_stereo;
}

void __near _sb_unset_hw(SBDEV *self)
{
    declare_Self;

    _Self->model = SBMODEL_UNKNOWN;
    _Self->dspv = 0;
    _Self->name = NULL;
    _Self->caps_flags = 0;
    _Self->caps_rate_mono = 0;
    _Self->caps_rate_stereo = 0;
}

bool __near _sb_set_hw_dsp(SBDEV *self, uint16_t dspv)
{
    declare_Self;
    int i;

    switch (dspv >> 8)
    {
    case 1:
    case 2:
    case 3:
    case 4:
        i = (dspv >> 8) - 1;
       _sb_set_hw(
            _Self,
            SBMODELS[i].model,
            dspv,
            SBMODELS[i].name,
            SBMODELS[i].caps,
            SBMODELS[i].rate_mono,
            SBMODELS[i].rate_stereo
        );
        return true;
    default:
        _sb_unset_hw(_Self);
        return false;
    }
}

void __near _sb_set_hw_flags(SBDEV *self, SBHWFLAGS flags)
{
    declare_Self;

    _Self->hw_flags = flags;
}

void __near _sb_unset_hw_flags(SBDEV *self)
{
    declare_Self;

    _Self->hw_flags = 0;
}

void __near _sb_set_hw_config(SBDEV *self, uint16_t base, uint8_t irq, uint8_t dma8, uint8_t dma16)
{
    declare_Self;

    _Self->hw_base = base;
    _Self->hw_irq = irq;
    _Self->hw_dma8 = dma8;
    _Self->hw_dma16 = dma16;
}

void __near _sb_unset_hw_config(SBDEV *self)
{
    declare_Self;

    _Self->hw_base = -1;
    _Self->hw_irq = -1;
    _Self->hw_dma8 = -1;
    _Self->hw_dma16 = -1;
}

bool __near _sb_hook_IRQ(SBDEV *self)
{
    declare_Self;

    DEBUG_BEGIN("_sb_hook_IRQ");

    if (!(_Self->hw_flags & SBHWFL_IRQ))
    {
        DEBUG_FAIL("_sb_hook_IRQ", "IRQ channel is not set.");
        return false;
    }

    if (!(_Self->hw_flags & SBHWFL_IRQ_HOOKED))
    {
        if (!hwowner_hook_irq(_sbdriver, _Self->hw_irq, &_ISR_play, (void *)_Self))
        {
            DEBUG_FAIL("_sb_hook_IRQ", "Failed to hook IRQ channel.");
            return false;
        }

        if (_Self->hw_irq != 2)  // no changes for IRQ 2
            if (!hwowner_enable_irq(_sbdriver, _Self->hw_irq))
            {
                DEBUG_FAIL("_sb_hook_IRQ", "Failed to enable IRQ channel.");
                return false;
            }

        _Self->hw_flags |= SBHWFL_IRQ_HOOKED;
    }

    DEBUG_SUCCESS("_sb_hook_IRQ");
    return true;
}

bool __near _sb_release_IRQ(SBDEV *self)
{
    declare_Self;

    DEBUG_BEGIN("_sb_release_IRQ");

    if (_Self->hw_flags & SBHWFL_IRQ_HOOKED)
    {
        if (_Self->hw_irq != 2)  // no changes for IRQ 2
            if (!hwowner_disable_irq(_sbdriver, _Self->hw_irq))
            {
                DEBUG_FAIL("_sb_release_IRQ", "Failed to disable IRQ channel.");
                return false;
            }

        if (!hwowner_release_irq(_sbdriver, _Self->hw_irq))
        {
            DEBUG_FAIL("_sb_release_IRQ", "Failed to release IRQ channel.");
            return false;
        }

        _Self->hw_flags &= ~SBHWFL_IRQ_HOOKED;
    }

    DEBUG_SUCCESS("_sb_release_IRQ");
    return true;
}

bool __near _sb_hook_DMA(SBDEV *self)
{
    declare_Self;

    DEBUG_BEGIN("_sb_hook_DMA");

    if (!(_Self->hw_flags & (SBHWFL_DMA8 | SBHWFL_DMA16)))
    {
        DEBUG_FAIL("_sb_hook_DMA", "DMA channels is not set.");
        return false;
    }

    if (_Self->hw_flags & SBHWFL_DMA8)
        if (!(_Self->hw_flags & SBHWFL_DMA8_HOOKED))
        {
            if (!hwowner_hook_dma(_sbdriver, _Self->hw_dma8))
            {
                DEBUG_FAIL("_sb_hook_DMA", "Failed to hook DMA8 channel.");
                return false;
            }
            _Self->hw_flags |= SBHWFL_DMA8_HOOKED;
        }

    if (_Self->hw_flags & SBHWFL_DMA16)
        if (!(_Self->hw_flags & SBHWFL_DMA16_HOOKED))
        {
            if (!hwowner_hook_dma(_sbdriver, _Self->hw_dma16))
            {
                DEBUG_FAIL("_sb_hook_DMA", "Failed to hook DMA16 channel.");
                return false;
            }
            _Self->hw_flags |= SBHWFL_DMA16_HOOKED;
        }

    DEBUG_SUCCESS("_sb_hook_DMA");
    return true;
}

bool __near _sb_release_DMA(SBDEV *self)
{
    declare_Self;

    DEBUG_BEGIN("_sb_release_DMA");

    if (_Self->hw_flags & SBHWFL_DMA8_HOOKED)
    {
        if (!hwowner_release_dma(_sbdriver, _Self->hw_dma8))
        {
            DEBUG_FAIL("_sb_release_DMA", "Failed to release DMA8 channel.");
            return false;
        }
        _Self->hw_flags &= ~SBHWFL_DMA8_HOOKED;
    }

    if (_Self->hw_flags & SBHWFL_DMA16_HOOKED)
    {
        if (!hwowner_release_dma(_sbdriver, _Self->hw_dma16))
        {
            DEBUG_FAIL("_sb_release_DMA", "Failed to release DMA16 channel.");
            return false;
        }
        _Self->hw_flags &= ~SBHWFL_DMA16_HOOKED;
    }

    DEBUG_SUCCESS("_sb_release_DMA");
    return true;
}

void __near _sb_unset_transfer_mode_DSP_command(SBDEV *self)
{
    declare_Self;

    _Self->transfer_flags &= ~SBTRFL_COMMANDS;
    _Self->transfer_mode_DSP_start = 0;
    _Self->transfer_mode_DSP_mode = 0;
}

void __near _sb_unset_transfer_mode(SBDEV *self)
{
    declare_Self;

    _Self->transfer_flags &= ~SBTRFL_MODE;
    _Self->transfer_mode_flags = 0;
    _Self->transfer_mode_rate = 0;
    _Self->transfer_mode_timeconst = 0;
    _Self->transfer_mode_channels = 0;
    _Self->transfer_mode_bits = 0;

    _sb_unset_transfer_mode_DSP_command(_Self);
}

void __near _sb_set_transfer_mode(SBDEV *self, bool f_mode, SBMODEFLAGS f_flags, uint16_t m_rate, uint8_t m_timeconst, uint8_t f_channels, uint8_t f_bits)
{
    declare_Self;

    if (f_mode)
        _Self->transfer_flags |= SBTRFL_MODE;
    else
        _Self->transfer_flags &= ~SBTRFL_MODE;

    _Self->transfer_mode_flags = f_flags;
    _Self->transfer_mode_rate = m_rate;
    _Self->transfer_mode_timeconst = m_timeconst;
    _Self->transfer_mode_channels = f_channels;
    _Self->transfer_mode_bits = f_bits;

    _sb_unset_transfer_mode_DSP_command(_Self);
}

void __near _sb_set_transfer_buffer(SBDEV *self, bool f_buffer, void *buffer, uint16_t frame_size, uint16_t frames_count, bool f_autoinit, void *callback, void *cb_param)
{
    declare_Self;

    if (f_buffer)
        _Self->transfer_flags |= SBTRFL_BUFFER;
    else
        _Self->transfer_flags &= ~SBTRFL_BUFFER;

    if (f_autoinit)
        _Self->transfer_flags |= SBTRFL_AUTOINIT;
    else
        _Self->transfer_flags &= ~SBTRFL_AUTOINIT;

    _Self->transfer_buffer = buffer;
    _Self->transfer_frame_size = frame_size;
    _Self->transfer_frames_count = frames_count;
    _Self->transfer_callback = callback;
    _Self->transfer_callback_param = cb_param;
}

void __near _sb_unset_transfer_buffer(SBDEV *self)
{
    declare_Self;

    _Self->transfer_flags &= ~(SBTRFL_BUFFER | SBTRFL_AUTOINIT);
    _Self->transfer_buffer = NULL;
    _Self->transfer_frame_size = 0;
    _Self->transfer_frames_count = 0;
    _Self->transfer_callback = NULL;
    _Self->transfer_callback_param = NULL;
}

bool __near _sb_setup_transfer_mode_DSP_commands(SBDEV *self)
{
    declare_Self;
    uint8_t cmd, mode;
    uint16_t samplerate, midrate;

    switch (_Self->model)
    {
    case SBMODEL_SB1:
        cmd = DSPC_DMA8_DAC;
        mode = 0;
        break;
    case SBMODEL_SB2:
    case SBMODEL_SBPRO:
        samplerate = _Self->transfer_mode_rate * _Self->transfer_mode_channels;

        midrate = (_Self->transfer_mode_channels == 2) ?
            calc_rate2(calc_time_const2(22050)) : calc_rate(calc_time_const(22050));

        if (samplerate < midrate)
            cmd = (_Self->transfer_flags & SBTRFL_AUTOINIT) ? DSPC_DMA8_DAC_AI : DSPC_DMA8_DAC;
        else
            cmd = (_Self->transfer_flags & SBTRFL_AUTOINIT) ? DSPC_DMA8_DAC_AI_HS : DSPC_DMA8_DAC_HS;

        mode = 0;
        break;
    case SBMODEL_SB16:
        cmd = SB16_DSPC_MODE_FIFO | SB16_DSPC_DIR_DAC;
        cmd |= (_Self->transfer_flags & SBTRFL_AUTOINIT) ? SB16_DSPC_DMA_AUTOINIT : SB16_DSPC_DMA_SINGLE;
        cmd |= (_Self->transfer_mode_bits == 16) ? SB16_DSPC_BITS_16 : SB16_DSPC_BITS_8;
        mode = (_Self->transfer_mode_flags & SBMODEFL_SIGNED) ? SB16_DSPM_SAMPLE_SIGNED : SB16_DSPM_SAMPLE_UNSIGNED;
        mode |= (_Self->transfer_mode_channels == 2) ? SB16_DSPM_CHANNELS_STEREO : SB16_DSPM_CHANNELS_MONO;
        break;
    default:
        DEBUG_FAIL("_sb_setup_transfer_mode_DSP_commands", "Unknown sound device.");
        return false;
    }

    _Self->transfer_mode_DSP_start = cmd;
    _Self->transfer_mode_DSP_mode = mode;

    _Self->transfer_flags |= SBTRFL_COMMANDS;
    return true;
}

bool __near _sb_start_DSP_transfer(SBDEV *self)
{
    declare_Self;
    uint16_t frame_len;
    uint8_t data[4];
    uint16_t length;

    if (!(_Self->transfer_flags & SBTRFL_COMMANDS))
        if (!_sb_setup_transfer_mode_DSP_commands(_Self))
        {
            DEBUG_FAIL("_sb_start_DSP_transfer", "Failed to setup DSP commands.");
            return false;
        }

    frame_len = _Self->transfer_frame_size;

    switch (_Self->model)
    {
    case SBMODEL_SB1:
        frame_len--;
        data[0] = _Self->transfer_mode_DSP_start;
        data[1] = frame_len & 0xff;
        data[2] = frame_len >> 8;
        length = 3;
        break;
    case SBMODEL_SB2:
    case SBMODEL_SBPRO:
        frame_len--;
        data[0] = DSPC_SET_SIZE;
        data[1] = frame_len & 0xff;
        data[2] = frame_len >> 8;
        data[3] = _Self->transfer_mode_DSP_start;
        length = 4;
        break;
    case SBMODEL_SB16:
        if (_Self->transfer_mode_bits == 16)
            frame_len >>= 1;
        frame_len--;
        data[0] = _Self->transfer_mode_DSP_start;
        data[1] = _Self->transfer_mode_DSP_mode;
        data[2] = frame_len & 0xff;
        data[3] = frame_len >> 8;
        length = 4;
        break;
    default:
        DEBUG_FAIL("_sb_start_DSP_transfer", "Unknown sound device.");
        return false;
    }

    if (!sbioDSPWriteQueue(_Self->hw_base, &data, length))
    {
        DEBUG_FAIL("_sb_start_DSP_transfer", "DSP I/O error.");
        return false;
    }

    return true;
}

// count: number of bytes (for 8-bits channel) or number of words (for 16-bits channel)
bool __near _sb_start_DMA_transfer(SBDEV *self)
{
    declare_Self;
    uint32_t count;
    DMAMODE mode;

    count = _Self->transfer_frame_size * _Self->transfer_frames_count;

    if (_Self->transfer_mode_bits == 16)
        count /= 2;

    mode = DMA_MODE_TRAN_READ | DMA_MODE_ADDR_INCR | DMA_MODE_SINGLE;
    mode |= (_Self->transfer_flags & SBTRFL_AUTOINIT) ? DMA_MODE_INIT_AUTO : DMA_MODE_INIT_SINGLE;

    return hwowner_setup_dma_transfer(
        _sbdriver,
        (_Self->transfer_mode_bits == 16) ? _Self->hw_dma16 : _Self->hw_dma8,
        mode,
        dma_get_linear_address(_Self->transfer_buffer),
        count
    );
}

void __near _sb_adjust_rate(SBDEV *self, uint16_t *rate, bool stereo, uint8_t *tc)
{
    declare_Self;
    uint8_t timeconst;

    if (stereo)
    {
        if (*rate < 4000)
            *rate = 4000;
        else
        if (*rate > _Self->caps_rate_stereo)
            *rate = _Self->caps_rate_stereo;
    }
    else
    {
        if (*rate < 4000)
            *rate = 4000;
        else
        if (*rate > _Self->caps_rate_mono)
            *rate = _Self->caps_rate_mono;
    }

    if ((_Self->model == SBMODEL_SB16) || !stereo)
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
    declare_Self;
    bool m_stereo;
    bool m_16bits;

    m_stereo = (*m_channels == 2) && (_Self->caps_flags & SBCAPS_STEREO);
    m_16bits = (*m_bits == 16) && (_Self->caps_flags & SBCAPS_16BITS);
    *f_sign = *f_sign && (_Self->caps_flags & SBCAPS_16BITS); // adjust f_sign

    _sb_adjust_rate(_Self, m_rate, m_stereo, m_tc);  // adjust m_rate

    *m_channels = m_stereo ? 2 : 1; // adjust m_channels
    *m_bits = m_16bits ? 16 : 8;    // adjust m_bits
}

bool __near _sb_transfer_stop(SBDEV *self);

bool __near _sb_transfer_start(SBDEV *self)
{
    declare_Self;
    uint8_t v;

    DEBUG_BEGIN("_sb_transfer_start");

    /* Check input parameters */

    if (!(_Self->hw_flags & SBHWFL_CONF))
    {
        DEBUG_FAIL("_sb_transfer_start", "Sound device is not configured.");
        return false;
    }

    if (!(_Self->transfer_flags & SBTRFL_BUFFER))
    {
        DEBUG_FAIL("_sb_transfer_start", "Transfer buffer is not set.");
        return false;
    }

    if (!(_Self->transfer_flags & SBTRFL_MODE))
    {
        DEBUG_FAIL("_sb_transfer_start", "Transfer mode is not set.");
        return false;
    }

    /* Hardware setup */

    if (!(_Self->hw_flags & SBHWFL_IRQ_HOOKED))
        if (!_sb_hook_IRQ(_Self))
        {
            DEBUG_FAIL("_sb_transfer_start", "Failed to hook IRQ channel.");
            return false;
        }

    if (((_Self->hw_flags & SBHWFL_DMA8) && (!(_Self->hw_flags & SBHWFL_DMA8_HOOKED)))
    ||  ((_Self->hw_flags & SBHWFL_DMA16) && (!(_Self->hw_flags & SBHWFL_DMA16_HOOKED))))
        if (!_sb_hook_DMA(_Self))
        {
            DEBUG_FAIL("_sb_transfer_start", "Failed to hook DMA channel(s).");
            return false;
        }

    sbioDSPAcknowledgeIRQ(_Self->hw_base, false);
    sbioDSPAcknowledgeIRQ(_Self->hw_base, true);
    _sb_transfer_stop(_Self);   // skip error

    if (_Self->model == SBMODEL_SB16)
        _sb_set_DSP_rate(_Self, _Self->transfer_mode_rate);
    else
    {
        _sb_set_DSP_time_constant(_Self, _Self->transfer_mode_timeconst);

        if (_Self->model == SBMODEL_SBPRO)
        {
            v = _sb_mixer_read(_Self, 0x0e) | 0x20; /* turn filter 'off' */

            if (_Self->transfer_mode_channels == 2)
                v |= 0x02;   /* turn stereo 'on' */

            _sb_mixer_write(_Self, 0x0e, v);
        }
    }

    _sb_set_speaker(_Self, true);

    if (!_sb_start_DMA_transfer(_Self))
    {
        DEBUG_FAIL("_sb_transfer_start", "Failed to start DMA transfer.");
        return false;
    }
    if (!_sb_start_DSP_transfer(_Self))
    {
        DEBUG_FAIL("_sb_transfer_start", "Failed to start DSP transfer.");
        return false;
    }

    _Self->transfer_flags |= SBTRFL_ACTIVE;

    DEBUG_SUCCESS("_sb_transfer_start");
    return true;
}

bool __near _sb_transfer_stop_8(SBDEV *self)
{
    declare_Self;
    uint8_t data[3];
    uint16_t length;

    switch (_Self->model)
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
        if (sbioDSPWriteQueue(_Self->hw_base, &data, length))
        {
            _Self->transfer_flags &= ~SBTRFL_ACTIVE;
            return true;
        }

    return false;
}

bool __near _sb_transfer_stop_16(SBDEV *self)
{
    declare_Self;
    uint8_t data[3];
    uint16_t length;

    length = 0;

    switch (_Self->model)
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
        if (sbioDSPWriteQueue(_Self->hw_base, &data, length))
        {
            _Self->transfer_flags &= ~SBTRFL_ACTIVE;
            return true;
        }

    return false;
}

bool __near _sb_transfer_stop(SBDEV *self)
{
    declare_Self;
    uint8_t count;
    uint8_t chn;
    uint8_t mask;

    if (!_sb_transfer_stop_8(_Self))
        return false;

    if (!_sb_transfer_stop_16(_Self))
        return false;

    /* reset is the best way to make sure SB stops playing */
    if (!sbioDSPReset(_Self->hw_base))
        return false;

    count = 0;
    chn = -1;
    mask = 0;

    if (_Self->hw_flags & SBHWFL_DMA8_HOOKED)
    {
        count++;
        chn = _Self->hw_dma8;
        mask |= 1 << _Self->hw_dma8;
    }

    if (_Self->hw_flags & SBHWFL_DMA16_HOOKED)
    {
        count++;
        chn = _Self->hw_dma16;
        mask |= 1 << _Self->hw_dma16;
    }

    if ((count == 1) || (_Self->hw_dma8 == _Self->hw_dma16))
    {
        if (!hwowner_mask_dma(_sbdriver, chn))
            return false;
    }
    else
    if (count > 1)
        if (!hwowner_mask_dma_channels(_sbdriver, mask))
            return false;

    _sb_set_speaker(_Self, false);
    _Self->transfer_flags &= ~SBTRFL_ACTIVE;
    return true;
}

bool __near _sb_detect_DSP_base(SBDEV *self)
{
    declare_Self;
    uint16_t p;
    uint16_t i;

    DEBUG_BEGIN("_sb_detect_DSP_base");

    if (_Self->hw_flags & SBHWFL_BASE)
    {
        DEBUG_SUCCESS("_sb_detect_DSP_base");
        return true;
    }

    i = 0;
    while ((!(_Self->hw_flags & SBHWFL_BASE)) && (i < HW_BASE_MAX))
    {
        p = HW_BASE_NUM[i];
        DEBUG_MSG_("_sb_detect_DSP_base", " - probing port 0x%04X...", p);

        if (sbioDSPReset(p))
        {
            _Self->hw_base = p;
            _Self->hw_flags |= SBHWFL_BASE;
        }

        i++;
    }

    if (!(_Self->hw_flags & SBHWFL_BASE))
    {
        DEBUG_FAIL("_sb_detect_DSP_base", "DSP not found.");
        return false;
    }

    if (_Self->hw_flags & SBHWFL_BASE)
        DEBUG_INFO_ ("_sb_detect_DSP_base", "Found DSP at base port 0x%04X.", p);

    _Self->dspv = _sb_read_DSP_version(_Self);
    if (sbioError != E_SBIO_SUCCESS)
    {
        DEBUG_FAIL("_sb_detect_DSP_base", "Unable to get DSP chip version.");
        return false;
    }

    DEBUG_INFO_("_sb_detect_DSP_base", "DSP version 0x%04X.", _Self->dspv);

    if (!_sb_set_hw_dsp(_Self, _Self->dspv))
    {
        DEBUG_FAIL("_sb_detect_DSP_base", "Unknown DSP chip version.");
        return false;
    }

    DEBUG_SUCCESS("_sb_detect_DSP_base");
    return true;
}

bool __near _sb_detect_IRQ(SBDEV *self, uint8_t dma, uint8_t bits)
{
    declare_Self;

    DEBUG_BEGIN("_sb_detect_IRQ");

    _Self->hw_irq = 0xff;

    if (bits == 16)
        _Self->hw_dma16 = dma;
    else
        _Self->hw_dma8 = dma;

    sb_transfer_stop(_Self);
    sb_set_transfer_buffer(_Self, (void *)&_sb_silence_u, 1, 1, false, NULL, NULL);
    sb_set_transfer_mode(_Self, 8000, 1, bits, false);
    sb_transfer_start(_Self);
    delay(10);

    if (_Self->hw_irq != 0xff)
    {
        DEBUG_INFO_ ("_sb_detect_IRQ", "Found IRQ channel %hu.", _Self->hw_irq);
        DEBUG_SUCCESS ("_sb_detect_IRQ");

        if (bits == 16)
            _Self->hw_flags |= SBHWFL_DMA16;
        else
            _Self->hw_flags |= SBHWFL_DMA8;

        _Self->hw_flags |= SBHWFL_IRQ;
        return true;
    }
    else
    {
        DEBUG_FAIL("_sb_detect_IRQ", "No DMA and IRQ channels were found.");
        return false;
    }
}

bool __near _sb_detect_DMA_IRQ(SBDEV *self, uint8_t bits)
{
    declare_Self;
    uint8_t i;
    uint8_t dmac;
    DMAMASK dmamask;
    IRQMASK irqmask;

    DEBUG_BEGIN("_sb_detect_DMA_IRQ");

    if (!(_Self->hw_flags & SBHWFL_BASE))
    {
        DEBUG_FAIL("_sb_detect_DMA_IRQ", "DSP base port is not set.");
        return false;
    }

    if (((bits != 16) && (_Self->hw_flags & SBHWFL_DMA8))
    ||  ((bits == 16) && (_Self->hw_flags & SBHWFL_DMA16)))
    {
        DEBUG_SUCCESS("_sb_detect_DMA_IRQ");
        return true;
    }

    dmamask = 0;
    for (i = 0; i < HW_DMA_MAX; i++)
        dmamask |= 1 << HW_DMA_NUM[i];

    dmamask &= ~dma_get_hooked_channels();
    if (!dmamask)
    {
        DEBUG_FAIL("_sb_detect_DMA_IRQ", "No free DMA channels are available.");
        return false;
    }

    if (!hwowner_hook_dma_channels(_sbdriver, dmamask))
    {
        DEBUG_FAIL("_sb_detect_DMA_IRQ", "Failed to hook DMA channel(s).");
        return false;
    }

    if (!hwowner_mask_dma_channels(_sbdriver, dmamask))
    {
        DEBUG_FAIL("_sb_detect_DMA_IRQ", "Failed to mask DMA channel(s).");
        return false;
    }

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

    if (!hwowner_hook_irq_channels(_sbdriver, irqmask, &_ISR_detect, _Self))
    {
        DEBUG_FAIL("_sb_detect_DMA_IRQ", "Failed to hook IRQ channel(s).");
        return false;
    }

    if (!hwowner_disable_irq_channels(_sbdriver, irqmask & ~(1 << 2)))  // no changes for IRQ 2
    {
        DEBUG_FAIL("_sb_detect_DMA_IRQ", "Failed to disable IRQ channels.");
        return false;
    }

    i = 0;
    while ((!(_Self->hw_flags & SBHWFL_DMA8)) && (i < HW_DMA_MAX))
    {
        dmac = HW_DMA_NUM[i];

        DEBUG_MSG_("_sb_detect_DMA_IRQ", "- trying DMA channel %hu...", dmac);

        _sb_detect_IRQ(_Self, dmac, bits);

        i++;
    }

    if (!hwowner_enable_irq_channels(_sbdriver, irqmask & ~(1 << 2)))   // no changes for IRQ 2
    {
        DEBUG_FAIL("_sb_detect_DMA_IRQ", "Failed to enable IRQ channels.");
        return false;
    }

    if (!hwowner_release_irq_channels(_sbdriver, irqmask))
    {
        DEBUG_FAIL("_sb_detect_DMA_IRQ", "Failed to release IRQ channels.");
        return false;
    }

    if (!hwowner_release_dma_channels(_sbdriver, dmamask))
    {
        DEBUG_FAIL("_sb_detect_DMA_IRQ", "Failed to release DMA channels.");
        return false;
    }

    sbioDSPReset(_Self->hw_base);   // skip error

    if (((bits != 16) && (_Self->hw_flags & SBHWFL_DMA8))
    ||  ((bits == 16) && (_Self->hw_flags & SBHWFL_DMA16)))
    {
        DEBUG_SUCCESS("_sb_detect_DMA_IRQ");
        return true;
    }

    DEBUG_FAIL("_sb_detect_DMA_IRQ", "DMA and IRQ channels were not found.");
    return false;
}

void __near _sb_free(SBDEV *self)
{
    declare_Self;

    DEBUG_BEGIN("_sb_free");

    if (_Self->transfer_flags & SBTRFL_ACTIVE)
        _sb_transfer_stop(_Self);   // skip error

    if (_Self->hw_flags & SBHWFL_IRQ_HOOKED)
        _sb_release_IRQ(_Self); // skip error

    if (_Self->hw_flags & (SBHWFL_DMA8_HOOKED | SBHWFL_DMA16_HOOKED))
        _sb_release_DMA(_Self); // skip error

    DEBUG_END("_sb_free");
}

/* Public methods, assuming 'self != NULL' */

SBDEV *sb_new(void)
{
    uint16_t seg;

    if (!_dos_allocmem(_dos_para(sizeof(struct sb_device_t)), &seg))
        return MK_FP(seg, 0);
    else
        return NULL;
}

void sb_init(SBDEV *self)
{
    declare_Self;

    memset(_Self, 0, sizeof(struct sb_device_t));
    /*
    _sb_unset_hw(_Self);
    _sb_unset_hw_flags(_Self);
    _sb_set_hw_config(_Self, 0x220, 7, 1, 5);
    _sb_unset_transfer_buffer(_Self);
    _sb_unset_transfer_mode(_Self);
    */
}

char *sb_get_name(SBDEV *self)
{
    declare_Self;

    return _Self->name;
}

uint8_t sb_mode_get_bits(SBDEV *self)
{
    declare_Self;

    return _Self->transfer_mode_bits;
}

bool sb_mode_is_signed(SBDEV *self)
{
    declare_Self;

    return _Self->transfer_mode_flags & SBMODEFL_SIGNED;
}

uint8_t sb_mode_get_channels(SBDEV *self)
{
    declare_Self;

    return _Self->transfer_mode_channels;
}

uint16_t sb_mode_get_rate(SBDEV *self)
{
    declare_Self;

    return _Self->transfer_mode_rate;
}

void sb_set_volume(SBDEV *self, uint8_t value)
{
    declare_Self;
    uint8_t b;

    if (_Self->caps_flags & SBCAPS_MIXER)
    {
        if (_Self->model == SBMODEL_SB16)
        {
            _sb_mixer_write(_Self, SBIO_MIXER_MASTER_LEFT, value);
            _sb_mixer_write(_Self, SBIO_MIXER_MASTER_RIGHT, value);
            _sb_mixer_write(_Self, SBIO_MIXER_VOICE_LEFT, value);
            _sb_mixer_write(_Self, SBIO_MIXER_VOICE_RIGHT, value);
        }
        else
        {
            if (value > 15)
                value = 15;
            value |= value << 4;
            _sb_mixer_write(_Self, SBIO_MIXER_MASTER_VOLUME, value);
            _sb_mixer_write(_Self, SBIO_MIXER_DAC_LEVEL, value);
        }
    }
}

bool sb_adjust_transfer_mode(SBDEV *self, uint16_t *m_rate, uint8_t *m_channels, uint8_t *m_bits, bool *f_sign)
{
    declare_Self;
    uint8_t m_timeconst;

    DEBUG_BEGIN("sb_adjust_transfer_mode");

    if (_Self->model != SBMODEL_UNKNOWN)
    {
        DEBUG_INFO_(
            "sb_adjust_transfer_mode",
            "(in) rate=%u, channels=%hu, bits=%hu, sign=%c.",
            *m_rate,
            *m_channels,
            *m_bits,
            *f_sign ? 'Y' : 'N'
        );

        _sb_adjust_transfer_mode(_Self, m_rate, &m_timeconst, m_channels, m_bits, f_sign);

        DEBUG_INFO_(
            "sb_adjust_transfer_mode",
            "(out) rate=%u, channels=%hu, bits=%hu, sign=%c.",
            *m_rate,
            *m_channels,
            *m_bits,
            *f_sign ? 'Y' : 'N'
        );

        DEBUG_SUCCESS("sb_adjust_transfer_mode");
        return true;
    }

    DEBUG_FAIL("sb_adjust_transfer_mode", "No sound device.");
    return false;
}

bool sb_set_transfer_mode(SBDEV *self, uint16_t m_rate, uint8_t m_channels, uint8_t m_bits, bool f_sign)
{
    declare_Self;

    uint8_t m_timeconst;

    DEBUG_BEGIN("sb_set_transfer_mode");

    if (_Self->model != SBMODEL_UNKNOWN)
    {
        _sb_adjust_transfer_mode(_Self, &m_rate, &m_timeconst, &m_channels, &m_bits, &f_sign);

        _Self->transfer_flags |= SBTRFL_MODE;

        if (f_sign)
            _Self->transfer_mode_flags |= SBMODEFL_SIGNED;
        else
            _Self->transfer_mode_flags &= ~SBMODEFL_SIGNED;

        _Self->transfer_mode_rate = m_rate;
        _Self->transfer_mode_timeconst = m_timeconst;
        _Self->transfer_mode_channels = m_channels;
        _Self->transfer_mode_bits = m_bits;

        DEBUG_SUCCESS("sb_set_transfer_mode");
        return true;
    }

    DEBUG_FAIL("sb_set_transfer_mode", "No sound device.");
    return false;
}

bool sb_set_transfer_buffer(SBDEV *self, void *buffer, uint16_t frame_size, uint16_t frames_count, bool autoinit, void *callback, void *cb_param)
{
    declare_Self;

    if (!(_Self->transfer_flags & SBTRFL_ACTIVE))
    {
        _sb_set_transfer_buffer(_Self, true, buffer, frame_size, frames_count, autoinit, callback, cb_param);
        return true;
    }

    return false;
}

bool sb_transfer_start(SBDEV *self)
{
    declare_Self;
    uint8_t v;

    DEBUG_BEGIN("sb_transfer_start");

    if (_sb_transfer_start(_Self))
    {
        DEBUG_SUCCESS("sb_transfer_start");
        return true;
    }

    DEBUG_FAIL("sb_transfer_start", "No sound device.");
    return false;
}

uint32_t sb_get_buffer_pos(SBDEV *self)
{
    declare_Self;

    uint32_t pos;

    if (_Self)
        if (_Self->transfer_flags & SBTRFL_ACTIVE)
        {
            pos = hwowner_get_dma_counter(_sbdriver, (_Self->transfer_mode_bits == 16) ? _Self->hw_dma16 : _Self->hw_dma8);

            if (_Self->transfer_mode_bits == 16)
                pos *= 2;

            return pos;
        }

    return 0;
}

bool sb_transfer_pause(SBDEV *self)
{
    declare_Self;

    if (_Self)
        if (_Self->transfer_flags & SBTRFL_ACTIVE)
            return sbioDSPWrite(_Self->hw_base,
                (_Self->transfer_mode_bits == 16) ? DSPC_DMA16_HALT : DSPC_DMA8_HALT);

    return false;
}

bool sb_transfer_continue(SBDEV *self)
{
    declare_Self;

    if (_Self)
        if (_Self->transfer_flags & SBTRFL_ACTIVE)
            return sbioDSPWrite(_Self->hw_base,
                (_Self->transfer_mode_bits == 16) ? DSPC_DMA16_CONTINUE : DSPC_DMA8_CONTINUE);

    return false;
}

bool sb_transfer_stop(SBDEV *self)
{
    declare_Self;

    return _sb_transfer_stop(_Self);
}

bool sb_conf_detect(SBDEV *self)
{
    declare_Self;
    uint16_t dspv;

    DEBUG_BEGIN("sb_conf_detect");

    _sb_unset_hw_flags(_Self);
    _sb_unset_hw(_Self);
    _sb_set_transfer_mode(_Self, true, 0, 8000, calc_time_const(8000), 1, 8);

    if (!_sb_detect_DSP_base(_Self))
    {
        DEBUG_FAIL("sb_conf_detect", "No DSP base I/O address specified.");
        return false;
    }

    _sb_transfer_stop(_Self);    // skip error

    if (!_sb_detect_DMA_IRQ(_Self, 8))
    {
        DEBUG_FAIL("sb_conf_detect", "Failed to find DMA and IRQ channels.");
        return false;
    }

    if (_Self->caps_flags & SBCAPS_16BITS)
        if (!_sb_detect_DMA_IRQ(_Self, 16))
        {
            DEBUG_FAIL("sb_conf_detect", "Failed to find DMA and IRQ channels.");
            return false;
        }

    _Self->hw_flags |= SBHWFL_CONF;

    DEBUG_SUCCESS("sb_conf_detect");
    return true;
}

void sb_conf_manual(SBDEV *self, SBCFGFLAGS flags, SBMODEL model, uint16_t base, uint8_t irq, uint8_t dma8, uint8_t dma16)
{
    declare_Self;
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

        _sb_set_hw_dsp(_Self, dspv);
        _sb_set_hw_flags(_Self, hwflags);
        _sb_set_hw_config(_Self, base, irq, dma8, dma16);
        _Self->hw_flags |= SBHWFL_CONF;
    }
    else
        _sb_unset_hw(_Self);

    _sb_unset_transfer_buffer(_Self);
    _sb_unset_transfer_mode(_Self);
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

bool sb_conf_input(SBDEV *self)
{
    declare_Self;
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

    sb_conf_manual(_Self, flags, model, base, irq, dma8, dma16);

    DEBUG_SUCCESS("sb_conf_input");
    return true;
}

bool sb_conf_env(SBDEV *self)
{
    declare_Self;
    char *envsb, *s, *param, *endptr;
    uint16_t s_seg;
    uint8_t type;
    uint16_t base;
    uint8_t irq, dma8, dma16;
    uint16_t len, i;
    SBCFGFLAGS flags;
    long int v;
    SBMODEL model;

    DEBUG_BEGIN("sb_conf_env");

    _sb_unset_hw(_Self);
    _sb_unset_hw_flags(_Self);
    _sb_unset_hw_config(_Self);
    _sb_unset_transfer_buffer(_Self);
    _sb_unset_transfer_mode(_Self);

    envsb = getenv("BLASTER");
    len = strlen(envsb);
    if (!len)
    {
        DEBUG_FAIL("sb_conf_env", "BLASTER environment variable is not set.");
        return false;
    }
    if (_dos_allocmem(_dos_para(len + 1), &s_seg))
    {
        DEBUG_FAIL("sb_conf_env", "Not enough DOS memory.");
        return false;
    }
    s = MK_FP(s_seg, 0);
    for (i = 0; i < len; i++)
        s[i] = toupper(envsb[i]);
    s[len] = 0;

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

    _cc_dos_freemem(s_seg);

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
        sb_conf_manual(_Self, flags, model, base, irq, dma8, dma16);
    }
    else
    {
        DEBUG_FAIL("sb_conf_env", "Configuration string is not complete.");
        return false;
    }

    DEBUG_SUCCESS("sb_conf_env");
    return true;
}

void sb_conf_dump(SBDEV *self)
{
    declare_Self;
    int i;

    printf("Sound device: " CRLF);
    i = _sb_find_model(_Self->model);
    if (i >= 0)
        printf("%s (%s).", SBMODELS[i].name, SBMODELS[i].comment);
    else
        printf("N/A." CRLF);

    printf("Hardware DSP base I/O address: ");
    if (_Self->hw_flags & SBHWFL_BASE)
        printf("0x%04X." CRLF, _Self->hw_base);
    else
        printf("N/A." CRLF);

    printf("Hardware IRQ channel: ");
    if (_Self->hw_flags & SBHWFL_IRQ)
        printf("%hu." CRLF, _Self->hw_irq);
    else
        printf("N/A." CRLF);

    printf("Hardware 8-bits DMA channel: ");
    if (_Self->hw_flags & SBHWFL_DMA8)
        printf("%hu." CRLF, _Self->hw_dma8);
    else
        printf("N/A." CRLF);

    printf("Hardware 16-bits DMA channel: ");
    if (_Self->hw_flags & SBHWFL_DMA16)
        printf("%hu." CRLF, _Self->hw_dma16);
    else
        printf("N/A." CRLF);
}

void sb_free(SBDEV *self)
{
    declare_Self;

    _sb_free(_Self);
}

void sb_delete(SBDEV **self)
{
    if (self)
        if (*self)
        {
            _sb_free(*self);
            _dos_freemem(FP_SEG(*self));
            *self = NULL;
        }
}

/*** Initialization ***/

void __near sbctl_init (void)
{
    DEBUG_BEGIN("sbctl_init");

    _sbdriver = hwowner_register("Internal SoundBlaster driver");

    DEBUG_END("sbctl_init");
}

void __near sbctl_done (void)
{
    DEBUG_BEGIN("sbctl_done");

    hwowner_unregister(_sbdriver);

    DEBUG_END("sbctl_done");
}

DEFINE_REGISTRATION (sbctl, sbctl_init, sbctl_done)

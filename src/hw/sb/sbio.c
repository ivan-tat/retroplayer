/* sbio.c -- Sound Blaster hardware i/o library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$hw$sb$sbio$*"
#endif

#include <stdbool.h>
#include <stdint.h>
#include "cc/conio.h"
#include "hw/sb/sbio.h"

/* Mixer i/o ports */

#define MIXER_REG  0x04
#define MIXER_DATA 0x05

/* DSP i/o ports */

#define DSP_RESET               0x06 // Read/Write
#define DSP_DATA_READ           0x0a // Read
#define DSP_DATA_AVAILABLE      0x0e // Read
#define DSP_ACKNOWLEDGE_IRQ8    0x0e // Read
#define DSP_ACKNOWLEDGE_IRQ16   0x0f // Read
#define DSP_WRITE_BUFFER_STATUS 0x0c // Read
#define DSP_WRITE_DATA          0x0c // Write

#if DEFINE_LOCAL_DATA == 1

SBIOERR sbioError;

#endif

void __far sbioMixerReset (uint16_t base)
{
    unsigned int wait;

    outp(base + MIXER_REG, 0);

    for (wait = 50; wait; wait--)
        __asm nop;

    outp(base + MIXER_DATA, 1);

    sbioError = E_SBIO_SUCCESS;
}

uint8_t __far sbioMixerRead (uint16_t base, uint8_t reg)
{
    outp(base + MIXER_REG, reg);
    sbioError = E_SBIO_SUCCESS;
    return inp(base + MIXER_DATA);
}

void __far sbioMixerWrite (uint16_t base, uint8_t reg, uint8_t data)
{
    uint8_t tmp;

    outp(base + MIXER_REG, reg);
    tmp = inp(base + MIXER_DATA);
    outp(base + MIXER_DATA, data);
    sbioError = E_SBIO_SUCCESS;
}

bool __far sbioDSPReset (uint16_t base)
{
    unsigned int count;
    uint8_t tmp;

    outp(base + DSP_RESET, 1);

    /* wait 3.3 microseconds */
    for (count = 5; count; count--)
        tmp = inp(base + DSP_RESET);

    outp(base + DSP_RESET, 0);

    for (count = 200; count; count--)
    {
        /* read data from DSP */
        tmp = sbioDSPRead(base);

        if (sbioError != E_SBIO_SUCCESS)
            break;

        if (tmp == 0xaa)
            return true;
    }

    sbioError = E_SBIO_DSP_RESET_FAILED;
    return false;
}

uint8_t __far sbioDSPRead (uint16_t base)
{
    unsigned int wait;

    for (wait = 0xffff; wait; wait--)
        if (inp(base + DSP_DATA_AVAILABLE) & 0x80)
        {
            sbioError = E_SBIO_SUCCESS;
            return inp(base + DSP_DATA_READ);
        }

    sbioError = E_SBIO_DSP_READ_FAILED;
    return 0;
}

bool __far sbioDSPReadQueue (uint16_t base, uint8_t *data, uint16_t length)
{
    uint8_t v;

    while (length)
    {
        v = sbioDSPRead(base);

        if (sbioError != E_SBIO_SUCCESS)
            return false;

        *data = v;
        data++;
        length--;
    }

    return true;
}

bool __far sbioDSPWrite (uint16_t base, uint8_t data)
{
    unsigned int wait;

    for (wait = 0xffff; wait; wait--)
        if (!(inp(base + DSP_WRITE_BUFFER_STATUS) & 0x80))
        {
            outp(base + DSP_WRITE_DATA, data);
            sbioError = E_SBIO_SUCCESS;
            return true;
        }

    sbioError = E_SBIO_DSP_WRITE_FAILED;
    return false;
}

bool __far sbioDSPWriteQueue (uint16_t base, uint8_t *data, uint16_t length)
{
    while (length)
    {
        if (!sbioDSPWrite(base, *data))
            return false;

        data++;
        length--;
    }

    return true;
}

void __far sbioDSPAcknowledgeIRQ (uint16_t base, bool mode16bit)
{
    unsigned char tmp;

    if (mode16bit)
        tmp = inp(base + DSP_ACKNOWLEDGE_IRQ16);
    else
        tmp = inp(base + DSP_ACKNOWLEDGE_IRQ8);
}

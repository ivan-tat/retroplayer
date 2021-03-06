/* sndctl_t.c -- type declarations for sndctl.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$hw$sndctl_t$*"
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "hw/sndctl_t.h"

bool __far set_sample_format (HWSMPFMT *p, uint8_t b, bool s, uint8_t c)
{
    HWSMPFMT fmt;

    switch (b)
    {
        case 8:
        case 16:
        case 32:
            fmt.flags = b | (s ? HWSMPFMTFL_SIGNED : 0);
            break;
    default:
        return false;
    }

    switch (c)
    {
        case 1:
        case 2:
            fmt.channels = c;
            break;
    default:
        return false;
    }

    *p = fmt;
    return true;
}

#define _sample_format_bits(p) (p->flags & HWSMPFMTFL_BITS_MASK)
#define _sample_format_signed(p) ((p->flags & HWSMPFMTFL_SIGNED) != 0)

uint8_t __far get_sample_format_bits (HWSMPFMT *p)
{
    return _sample_format_bits(p);
}

bool __far is_sample_format_signed (HWSMPFMT *p)
{
    return _sample_format_signed(p);
}

uint8_t __far get_sample_format_channels (HWSMPFMT *p)
{
    return p->channels;
}

uint16_t __far get_sample_format_width (HWSMPFMT *p)
{
    return (_sample_format_bits(p) >> 3) * p->channels;
}

void __far clear_sample_format (HWSMPFMT *p)
{
    p->flags = 0;
    p->channels = 0;
}

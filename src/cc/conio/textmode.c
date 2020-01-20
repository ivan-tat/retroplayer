/* textmode.c -- part of custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$conio$textmode$*"
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "cc/i86.h"
#include "hw/vbios.h"
#include "cc/conio.h"

void cc_textmode(uint16_t mode)
{
    _cc_console_set_mode(mode);
    _cc_console_on_mode_change();
    cc_textattr = cc_textattrorig;
}

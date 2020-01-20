/* outp.c -- part of custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$conio$outp$*"
#endif  /* __WATCOMC__ */

#include "cc/conio.h"

#ifndef __WATCOMC__

void cc_outp(unsigned int port, char value)
{
    _asm
    {
        "mov dx,port";
        "mov al,value";
        "out dx,al";
    }
}

#endif  /* !__WATCOMC__ */

/* inp.c -- part of custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "cc/conio.h"

#ifndef __WATCOMC__

char cc_inp(unsigned int port)
{
    _asm
    {
        "mov dx,port";
        "in al,dx";
    }
}

#endif  /* __WATCOMC__ */

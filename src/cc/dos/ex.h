/* ex.h -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _CC_DOS_EX_H_INCLUDED
#define _CC_DOS_EX_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdint.h>
#include "cc/dos.h"

extern uint16_t __cdecl __far _cc_dos_exec_asm (const char *path, struct cc_dos_execparam_t *param);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _cc_dos_exec_asm "_*";

#endif  /* __WATCOMC__ */

#endif  /* !_CC_DOS_EX_H_INCLUDED */

/* ex.h -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_DOS_EX_H
#define CC_DOS_EX_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdint.h>
#include "cc/dos.h"

extern uint16_t __cdecl __far _cc_dos_exec_asm (const char *path, struct cc_dos_execparam_t *param);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _cc_dos_exec_asm "_*";

#endif  /* __WATCOMC__ */

#endif  /* CC_DOS_ERROR086_H */

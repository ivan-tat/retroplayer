/* smalls3m.h -- declarations for smalls3m.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _SMALLS3M_H_INCLUDED
#define _SMALLS3M_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"

#if LINKER_TPC != 1
void __noreturn _start_c (void);
int main (int argc, const char **argv);
#endif  /* LINKER_TPC != 1 */

void __far smalls3m_main (void);

/*** Initialization ***/

DECLARE_REGISTRATION (smalls3m)

/*** Linking ***/

#ifdef __WATCOMC__

#if LINKER_TPC != 1
#pragma aux _start_c "*";
#pragma aux main "*";
#endif  /* LINKER_TPC != 1 */

#pragma aux smalls3m_main "*";

#pragma aux register_smalls3m "*";
#pragma aux unregister_smalls3m "*";

#endif  /* __WATCOMC__ */

#endif  /* !_SMALLS3M_H_INCLUDED */

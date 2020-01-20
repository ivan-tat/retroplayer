/* playosci.h -- declarations for playosci.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _PLAYOSCI_H_INCLUDED
#define _PLAYOSCI_H_INCLUDED 1

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

void __far playosci_main (void);

/*** Initialization ***/

DECLARE_REGISTRATION (playosci)

/*** Linking ***/

#ifdef __WATCOMC__

#if LINKER_TPC != 1
#pragma aux _start_c "*";
#pragma aux main "*";
#endif  /* LINKER_TPC != 1 */

#pragma aux playosci_main "*";

#pragma aux register_playosci "*";
#pragma aux unregister_playosci "*";

#endif  /* __WATCOMC__ */

#endif  /* !_PLAYOSCI_H_INCLUDED */

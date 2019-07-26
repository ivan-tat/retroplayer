/* playosci.h -- declarations for playosci.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef PLAYOSCI_H
#define PLAYOSCI_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"

void __far playosci_main (void);

/*** Initialization ***/

DECLARE_REGISTRATION (playosci)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux playosci_main "*";

#pragma aux register_playosci "*";
#pragma aux unregister_playosci "*";

#endif  /* __WATCOMC__ */

#endif  /* PLAYOSCI_H */

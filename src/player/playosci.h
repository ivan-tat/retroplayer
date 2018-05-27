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

/* Linking */

#ifdef __WATCOMC__
#pragma aux playosci_main "*";
#endif

#endif  /* PLAYOSCI_H */

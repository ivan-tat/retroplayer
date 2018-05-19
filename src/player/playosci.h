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

extern uint16_t opt_rate;
extern bool opt_stereo;
extern bool opt_16bits;
extern bool opt_lq;
extern char opt_filename[pascal_String_size];
extern uint16_t drawseg;

void PUBLIC_CODE playosci_init (void);
//void PUBLIC_CODE playosci_main (void);

/*** Initialization ***/

DECLARE_REGISTRATION(playosci)

/* Linking */

#ifdef __WATCOMC__
#pragma aux opt_rate "*";
#pragma aux opt_stereo "*";
#pragma aux opt_16bits "*";
#pragma aux opt_lq "*";
#pragma aux opt_filename "*";
#pragma aux drawseg "*";
#pragma aux playosci_init "*";
#endif

#endif  /* PLAYOSCI_H */

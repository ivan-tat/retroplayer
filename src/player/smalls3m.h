/* smalls3m.h -- declarations for smalls3m.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef SMALLS3M_H
#define SMALLS3M_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"

void __far smalls3m_main (void);

/*** Initialization ***/

DECLARE_REGISTRATION (smalls3m)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux smalls3m_main "*";

#pragma aux register_smalls3m "*";
#pragma aux unregister_smalls3m "*";

#endif  /* __WATCOMC__ */

#endif  /* SMALLS3M_H */

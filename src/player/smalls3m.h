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

void PUBLIC_CODE smalls3m_main(void);

#endif  /* SMALLS3M_H */

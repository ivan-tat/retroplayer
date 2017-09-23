/* detisr.h -- declarations for detisr.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _DETISR_H
#define _DETISR_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

#include "hw/sb/detisr_.h"

void PUBLIC_CODE SetDetISRCallback(DetISRCallback_t *p);

#endif /* _DETISR_H */

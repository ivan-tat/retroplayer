/* mixing.h -- declarations for mixing.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MIXING_H
#define MIXING_H 1

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

void    *PUBLIC_CODE mapSampleData(uint16_t seg, uint16_t len);
void     PUBLIC_CODE calcTick(void *outBuf, uint16_t len);

#endif  /* MIXING_H */

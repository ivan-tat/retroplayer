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
uint16_t PUBLIC_CODE getBufOffFromCount(uint16_t count);
uint16_t PUBLIC_CODE getCountFromBufOff(uint16_t bufOff);
void     PUBLIC_CODE calcTick(void);

#endif  /* MIXING_H */

/* dos_.h -- declarations for dos_.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CUSTOM_DOS_H
#define CUSTOM_DOS_H 1

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

#include "pascal/pascal.h"

#define _dos_para(size) (((size) + 15) >> 4)

uint16_t PUBLIC_CODE _dos_allocmem(uint16_t size, uint16_t *seg);
uint16_t PUBLIC_CODE _dos_freemem(uint16_t seg);
uint16_t PUBLIC_CODE _dos_setblock(uint16_t size, uint16_t seg, uint16_t *max);

#endif  /* CUSTOM_DOS_H */

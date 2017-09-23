/* posttab.h -- declarations for posttab.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef POSTTAB_H
#define POSTTAB_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

extern uint8_t  PUBLIC_DATA post8bit[4096];
extern uint16_t PUBLIC_DATA post16bit[4096];

void PUBLIC_CODE calcPostTable(uint8_t vol, bool use16bit);

#endif  /* POSTTAB_H */

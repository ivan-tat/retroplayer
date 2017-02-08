/* i86.h -- declarations for i86.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CUSTOM_I86_H
#define CUSTOM_I86_H 1

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

#include "..\pascal\pascal.h"

extern void intr(int, void *);
void delay(unsigned int __milliseconds);

#endif  /* CUSTOM_I86_H */

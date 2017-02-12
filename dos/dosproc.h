/* dosproc.h -- declarations for dosproc.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef DOSPROC_H
#define DOSPROC_H 1

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

#include "..\pascal\pascal.h"

bool     PUBLIC_CODE getdosmem(void **p, uint32_t size);
void     PUBLIC_CODE freedosmem(void **p);
bool     PUBLIC_CODE setsize(void *p, uint32_t size);
uint32_t PUBLIC_CODE getfreesize(void);

#endif  /* DOSPROC_H */

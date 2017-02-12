/* malloc.c -- custom memory allocation library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "..\pascal\pascal.h"
#include "malloc.h"

uint32_t _memmax(void)
{
    return pascal_maxavail();
};

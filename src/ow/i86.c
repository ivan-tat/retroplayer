/* i86.c -- custom i86 library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "pascal/pascal.h"
#include "ow/i86.h"

void delay(unsigned int __milliseconds)
{
    pascal_delay(__milliseconds);
}

/* detisr.c -- hardware IRQ detection library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

#include "hw/sb/detisr.h"

void PUBLIC_CODE SetDetISRCallback(DetISRCallback_t *p)
{
    DetISRCallback = p;
}

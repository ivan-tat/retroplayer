/* error086.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/dos.h"
#include "cc/dos/dosret.h"
#include "cc/dos/error086.h"

unsigned __cc_doserror(unsigned code)
{
    if (code)
        return __cc_set_errno_dos(code);
    else
        return 0;
}

/* musins.c -- musical instrument handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/dos.h"
#include "dynarray.h"
#include "dos/ems.h"

#include "main/musins.h"

/*** Musical instrument ***/


/*** Musical instruments list ***/

#ifdef DEFINE_LOCAL_DATA

/* instruments */

instrumentsList_t *PUBLIC_DATA mod_Instruments;
uint16_t PUBLIC_DATA InsNum;
bool     PUBLIC_DATA EMSSmp;
uint16_t PUBLIC_DATA SmpEMSHandle;

#endif  /* DEFINE_LOCAL_DATA */

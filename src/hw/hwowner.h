/* hwowner.h -- declarations for hwowner.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef HWOWNER_H
#define HWOWNER_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

/* Hardware resource owner */

typedef uint16_t hw_owner_id_t;
typedef hw_owner_id_t HWOWNERID;
typedef void HWOWNER;

HWOWNER    *hwowner_register(const char *name);
HWOWNERID   hwowner_get_id(HWOWNER *owner);
const char *hwowner_get_name(HWOWNERID id);
void        hwowner_unregister(HWOWNER *owner);

/*** Initialization ***/

DECLARE_REGISTRATION (hwowner)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux hwowner_register "*";
#pragma aux hwowner_get_id "*";
#pragma aux hwowner_get_name "*";
#pragma aux hwowner_unregister "*";

#pragma aux register_hwowner "*";
#pragma aux unregister_hwowner "*";

#endif  /* __WATCOMC__ */

#endif  /* HWOWNER_H */

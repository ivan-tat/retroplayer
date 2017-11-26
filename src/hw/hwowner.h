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

typedef struct hw_owner_t
{
    uint16_t id;
    const char *name;
};
typedef struct hw_owner_t HWOWNER;

void hwowner_clear(HWOWNER *self);
void hwowner_init(HWOWNER *self, const char *name);
void hwowner_free(HWOWNER *self);

#ifdef __WATCOMC__
#pragma aux hwowner_clear "*";
#pragma aux hwowner_init "*";
#pragma aux hwowner_free "*";
#endif

/* Initialization */

DECLARE_REGISTRATION(hwowner)

#endif  /* HWOWNER_H */

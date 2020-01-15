/* _env.h -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_STDLIB_ENV_H
#define CC_STDLIB_ENV_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#pragma pack(push, 1);
typedef struct dosenvlist_t
{
    char *arr;
    uint16_t size;
};
#pragma pack(pop);

#pragma pack(push, 1);
typedef struct envstrlist_t
{
    char **arr;
    uint16_t size;  /* count including terminating NULL */
};
#pragma pack(pop);

/* Global variables */

extern struct dosenvlist_t _dos_env;
extern struct envstrlist_t _env_list;

/* DOS program segment prefix */

void _psp_get_dosenv(uint16_t self, struct dosenvlist_t *env);

/* DOS environment */

bool     _dosenv_next(struct dosenvlist_t *self, uint16_t *i);
uint16_t _dosenv_find_end(struct dosenvlist_t *self, uint16_t *count);
char    *_dosenv_find(struct dosenvlist_t *self, const char *name, uint16_t nlen);

/* Internal environment variables list */

/* Returns zero (false) on success */
bool _env_name_check(const char *name, uint16_t *nlen);

void _environ_clear(struct envstrlist_t *self);
bool _environ_alloc(struct envstrlist_t *self, uint16_t count);
void _environ_fill(struct envstrlist_t *self, struct dosenvlist_t *dosenv);
void _environ_free(struct envstrlist_t *self);

/* Semi-public environment variables list */

void cc_environ_clear(void);
bool cc_environ_build(void);
bool cc_environ_rebuild(void);
bool cc_environ_init(void);
void cc_environ_free(void);

/* Publics */

/*
// defined in "stdlib.h":
bool environ_init(void);
void environ_free(void);
*/

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _dos_env "*";
#pragma aux _env_list "*";

#pragma aux _dos_env "*";
#pragma aux _psp_get_dosenv "*";
#pragma aux _dosenv_next "*";
#pragma aux _dosenv_find_end "*";
#pragma aux _dosenv_find "*";
#pragma aux _env_name_check "*";
#pragma aux _environ_clear "*";
#pragma aux _environ_alloc "*";
#pragma aux _environ_fill "*";
#pragma aux _environ_free "*";
#pragma aux cc_environ_clear "*";
#pragma aux cc_environ_build "*";
#pragma aux cc_environ_rebuild "*";
#pragma aux cc_environ_init "*";
#pragma aux cc_environ_free "*";

#endif  /* __WATCOMC__ */

#endif

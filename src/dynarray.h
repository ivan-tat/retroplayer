/* dynarray.h -- declarations for dynamic array handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _DYNARRAY_H_INCLUDED
#define _DYNARRAY_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"

/*** Dynamic array virtual methods ***/

typedef void __far dynarr_init_item_t(void *parent, void *item);
typedef void __far dynarr_free_item_t(void *parent, void *item);

/*** Dynamic array structure ***/

#pragma pack(push, 1);
typedef struct dynarr_t
{
    void *parent;
    uint16_t item_size;
    uint16_t size;
    void *list;
    dynarr_init_item_t *init_item;
    dynarr_free_item_t *free_item;
};
#pragma pack(pop);
typedef struct dynarr_t DYNARR;

void     __far dynarr_init(DYNARR *self, void *parent, uint16_t item_size,
            dynarr_init_item_t init_item,
            dynarr_free_item_t free_item);
void     __far dynarr_init_items(DYNARR *self, uint16_t index, uint16_t count);
void     __far dynarr_free_items(DYNARR *self, uint16_t index, uint16_t count);
void     __far dynarr_set_item(DYNARR *self, uint16_t index, void *item);
void    *__far dynarr_get_item(DYNARR *self, uint16_t index);
int32_t  __far dynarr_indexof(DYNARR *self, void *item);
bool     __far dynarr_set_size(DYNARR *self, uint16_t size);
uint16_t __far dynarr_get_size(DYNARR *self);
void     __far dynarr_free(DYNARR *self);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux dynarr_init "*";
#pragma aux dynarr_init_items "*";
#pragma aux dynarr_free_items "*";
#pragma aux dynarr_set_item "*";
#pragma aux dynarr_get_item "*";
#pragma aux dynarr_indexof "*";
#pragma aux dynarr_set_size "*";
#pragma aux dynarr_get_size "*";
#pragma aux dynarr_free "*";

#endif  /* __WATCOMC__ */

#endif  /* !_DYNARRAY_H_INCLUDED */

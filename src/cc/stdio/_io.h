/* _io.h -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_STDIO__IO_H
#define CC_STDIO__IO_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

/* File I/O */

extern void     PUBLIC_CODE pascal_assign(PASCALFILE *f, const char *path);
extern bool     PUBLIC_CODE pascal_reset(PASCALFILE *f);
extern bool     PUBLIC_CODE pascal_rewrite(PASCALFILE *f);
extern void     PUBLIC_CODE pascal_close(PASCALFILE *f);
extern bool     PUBLIC_CODE pascal_seek(PASCALFILE *f, uint32_t pos);
extern uint16_t PUBLIC_CODE pascal_blockread(PASCALFILE *f, void *buf, uint16_t size, uint16_t *actual);
extern uint16_t PUBLIC_CODE pascal_blockwrite(PASCALFILE *f, void *buf, uint16_t size, uint16_t *actual);

#ifdef __WATCOMC__
#pragma aux pascal_assign     modify [ax bx cx dx si di es];
#pragma aux pascal_reset      modify [   bx cx dx si di es];
#pragma aux pascal_rewrite    modify [   bx cx dx si di es];
#pragma aux pascal_close      modify [ax bx cx dx si di es];
#pragma aux pascal_seek       modify [   bx cx dx si di es];
#pragma aux pascal_blockread  modify [   bx cx dx si di es];
#pragma aux pascal_blockwrite modify [   bx cx dx si di es];
#endif

#endif  /* CC_STDIO__IO_H */

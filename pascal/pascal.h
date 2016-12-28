/* pascal.h -- support for pascal linker.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef PASCAL_H
#define PASCAL_H 1

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

#define PUBLIC_DATA __pascal
#define PUBLIC_CODE __far __pascal
#define EXTERN_LINK extern

/*** System unit ***/

/* Initialization */

extern void *PUBLIC_DATA exitproc;

/* Use init()/done() methods in C only */
/* Use register_*()/unregister_*() methods in Pascal only */

#define _REGNAME(name)   register_##name
#define _UNREGNAME(name) unregister_##name

#define _REGMETHOD(name)   void PUBLIC_CODE _REGNAME(name)(void)
#define _UNREGMETHOD(name) void PUBLIC_CODE _UNREGNAME(name)(void)

/* Use DECLARE_INITIALIZATION() in header files */
/* Use DECLARE_REGISTRATION() in header files */
/* Use DEFINE_REGISTRATION() in C files */

#define DECLARE_INITIALIZATION(init, done)\
void init(void);\
void done(void);

#define DECLARE_REGISTRATION(name)\
_REGMETHOD(name);\
_UNREGMETHOD(name);

#define _EXITVARNAME(name) _oldexit_##name

#ifdef DEBUG
 #define LOG_REGMETHOD(name, init) printf("[init] " #name ": " #init "()\r\n");
 #define LOG_UNREGMETHOD(name, done) printf("[done] " #name ": " #done "()\r\n");
#else
 #define LOG_REGMETHOD(name, init)
 #define LOG_UNREGMETHOD(name, done)
#endif

#define DEFINE_REGISTRATION(name, init, done) \
static void *_EXITVARNAME(name) = (void *)0;\
\
_REGMETHOD(name)\
{\
    LOG_REGMETHOD(name, init)\
    init();\
    _EXITVARNAME(name) = exitproc;\
    exitproc = _UNREGNAME(name);\
}\
\
_UNREGMETHOD(name)\
{\
    LOG_UNREGMETHOD(name, done)\
    done();\
    exitproc = _EXITVARNAME(name);\
}

/* Heap */

extern uint32_t PUBLIC_CODE mavail(void);
extern void    *PUBLIC_CODE malloc(uint16_t size);
extern void     PUBLIC_CODE memfree(void *p, uint16_t size);

/*** CRT unit ***/

extern void PUBLIC_CODE crt_delay(uint16_t count);
#define DELAY(count) crt_delay(count)

#endif  /* PASCAL_H */

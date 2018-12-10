/* pascal.h -- support for pascal linker.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef PASCAL_H
#define PASCAL_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "startup.h"

/*** Initialization ***/

/*extern void *__pascal exitproc;*/

/* Use init()/done() methods in C only */
/* Use register_*()/unregister_*() methods in Pascal only */

#define _REGNAME(name)   register_##name
#define _UNREGNAME(name) unregister_##name

#define _REGMETHOD(name)   void __far _REGNAME(name)(void)
#define _UNREGMETHOD(name) void __far _UNREGNAME(name)(void)

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
 #define LOG_REGMETHOD(name, init)   DEBUG_REG(#init, #name);
 #define LOG_UNREGMETHOD(name, done) DEBUG_UNREG(#done, #name);
#else
 #define LOG_REGMETHOD(name, init)
 #define LOG_UNREGMETHOD(name, done)
#endif

#define DEFINE_REGISTRATION(name, init, done) \
/*static void *_EXITVARNAME(name) = (void *)0;*/\
\
_REGMETHOD(name)\
{\
    LOG_REGMETHOD(name, init)\
    init();\
    cc_atexit((void *__far)_UNREGNAME(name));\
    /*_EXITVARNAME(name) = exitproc;*/\
    /*exitproc = _UNREGNAME(name);*/\
}\
\
_UNREGMETHOD(name)\
{\
    LOG_UNREGMETHOD(name, done)\
    done();\
    /*exitproc = _EXITVARNAME(name);*/\
}

#define pascal_String_size 256

extern void __far __pascal pascal_fillchar (void *dest, uint16_t size, uint8_t value);
extern void __far __pascal pascal_move (void *src, void *dest, uint16_t size);

void __far __pascal strpastoc (char *dest, char const *src, uint16_t maxlen);
void __far __pascal strctopas (char *dest, char const *src, uint16_t maxlen);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux pascal_fillchar "*" modify [ ax bx cx dx si di es ];
#pragma aux pascal_move     "*" modify [ ax bx cx dx si di es ];

#pragma aux strpastoc "*";
#pragma aux strctopas "*";

#endif  /* __WATCOMC__ */

#endif  /* PASCAL_H */

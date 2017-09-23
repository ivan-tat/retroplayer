/* pascal.h -- support for pascal linker.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef PASCAL_H
#define PASCAL_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

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
 #define LOG_REGMETHOD(name, init)   DEBUG_REG(#init, #name);
 #define LOG_UNREGMETHOD(name, done) DEBUG_UNREG(#done, #name);
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

/* File I/O */

typedef struct pascalFile_t {
    char data[128];
};
typedef struct pascalFile_t PASCALFILE;

#endif  /* PASCAL_H */

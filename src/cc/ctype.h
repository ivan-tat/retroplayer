/* ctype.h -- declarations for custom character handling.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_CTYPE_H
#define CC_CTYPE_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

#define _CC_CNTRL  0x01
#define _CC_SPACE  0x02
#define _CC_PUNCT  0x04
#define _CC_PRINT  0x08
#define _CC_XDIGT  0x10
#define _CC_DIGIT  0x20
#define _CC_UPPER  0x40
#define _CC_LOWER  0x80

extern const unsigned char _cc_IsTable[257];

#define cc_isalnum(__c)  ((unsigned char)(_cc_IsTable[((unsigned char)(__c))+1]) & (_CC_LOWER|_CC_UPPER|_CC_DIGIT))
#define cc_isalpha(__c)  ((unsigned char)(_cc_IsTable[((unsigned char)(__c))+1]) & (_CC_LOWER|_CC_UPPER))
#define cc_iscntrl(__c)  ((unsigned char)(_cc_IsTable[((unsigned char)(__c))+1]) & _CC_CNTRL)
#define cc_isdigit(__c)  ((unsigned char)(_cc_IsTable[((unsigned char)(__c))+1]) & _CC_DIGIT)
#define cc_isgraph(__c) (((unsigned char)(_cc_IsTable[((unsigned char)(__c))+1]) & (_CC_PRINT|_CC_SPACE))==_CC_PRINT)
#define cc_islower(__c)  ((unsigned char)(_cc_IsTable[((unsigned char)(__c))+1]) & _CC_LOWER)
#define cc_isprint(__c)  ((unsigned char)(_cc_IsTable[((unsigned char)(__c))+1]) & _CC_PRINT)
#define cc_ispunct(__c)  ((unsigned char)(_cc_IsTable[((unsigned char)(__c))+1]) & _CC_PUNCT)
#define cc_isspace(__c)  ((unsigned char)(_cc_IsTable[((unsigned char)(__c))+1]) & _CC_SPACE)
#define cc_isupper(__c)  ((unsigned char)(_cc_IsTable[((unsigned char)(__c))+1]) & _CC_UPPER)
#define cc_isxdigit(__c) ((unsigned char)(_cc_IsTable[((unsigned char)(__c))+1]) & _CC_XDIGT)

/*** Aliases ***/

#define _CNTRL _CC_CNTRL
#define _SPACE _CC_SPACE
#define _PUNCT _CC_PUNCT
#define _PRINT _CC_PRINT
#define _XDIGT _CC_XDIGT
#define _DIGIT _CC_DIGIT
#define _UPPER _CC_UPPER
#define _LOWER _CC_LOWER

#define _IsTable _cc_IsTable

#define isalnum cc_isalnum
#define isalpha cc_isalpha
#define iscntrl cc_iscntrl
#define isdigit cc_isdigit
#define isgraph cc_isgraph
#define islower cc_islower
#define isprint cc_isprint
#define ispunct cc_ispunct
#define isspace cc_isspace
#define isupper cc_isupper
#define isxdigit cc_isxdigit

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _cc_IsTable "*";

#endif  /* __WATCOMC__ */

#endif  /* CC_CTYPE_H */

/* share.h -- declarations for custom "share" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _CC_SHARE_H_INCLUDED
#define _CC_SHARE_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdint.h>

/* Access/Open mode bit-flags */

#define CC_SH_COMPAT 0      // compatibility mode
#define CC_SH_DENYNO 0x40   // deny none
#define CC_SH_DENYRD 0x30   // deny read
#define CC_SH_DENYWR 0x20   // deny write
#define CC_SH_DENYRW 0x10   // deny read and write (exclusive)

/*** Aliases ***/

#define SH_COMPAT CC_SH_COMPAT
#define SH_DENYNO CC_SH_DENYNO
#define SH_DENYRD CC_SH_DENYRD
#define SH_DENYRW CC_SH_DENYRW
#define SH_DENYWR CC_SH_DENYWR

#endif  /* !_CC_SHARE_H_INCLUDED */

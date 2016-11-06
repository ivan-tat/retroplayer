/* sndisr_.h -- declarations for sndisr_.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _SNDISR__H
#define _SNDISR__H 1

typedef void __far __pascal SoundHWISRCallback_t( void );

extern SoundHWISRCallback_t * __pascal SoundHWISRCallback;

extern void __far __pascal SoundHWISR( void );

#endif /* _SNDISR__H */

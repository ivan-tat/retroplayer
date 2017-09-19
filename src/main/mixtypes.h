/* mixtypes.h -- mixer types.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MIXTYPES_H
#define MIXTYPES_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#define MIXSMPFL_16BITS 0x01
#define MIXSMPFL_LOOP   0x02

typedef struct playSampleInfo_t {
    void    *dData;
    uint32_t dPos;
    uint32_t dStep;
    uint16_t wLen;
    uint16_t wLoopStart;
    uint16_t wLoopEnd;
    uint16_t wFlags;
};

#endif  /* MIXTYPES_H */

#ifndef _mixer__h_included
#define _mixer__h_included 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

extern uint32_t __far __pascal _mixCalcSampleStep( void );
extern void     __far __pascal _MixSampleMono8( void );
extern void     __far __pascal _MixSampleStereo8( void );

#endif /* _mixer__h_included */

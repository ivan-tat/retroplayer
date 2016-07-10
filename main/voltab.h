#ifndef _VOLTAB_H
#define _VOLTAB_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

typedef int16_t voltab_t[65][256];

extern voltab_t * __pascal volumetableptr;

void __far __pascal initVolumeTable( void );
bool __far __pascal allocVolumeTable( void );
void __far __pascal calcVolumeTable( bool sign );
void __far __pascal freeVolumeTable( void );

#endif /* _VOLTAB_H */

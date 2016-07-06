#ifndef _SBCTL_H
#define _SBCTL_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

bool __far __pascal sbioDSPWrite( uint16_t base, uint8_t data );

#endif /* _SBCTL_H */

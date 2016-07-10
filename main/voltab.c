#include "voltab.h"
#include "..\dos\dosproc.h"

#ifdef __WATCOMC__
#include <string.h>
#endif

void __far __pascal initVolumeTable( void ) {
    volumetableptr = NULL;
}

bool __far __pascal allocVolumeTable( void )
{
	if( getdosmem( &volumetableptr, sizeof( voltab_t ) ) )
	{
		memset( volumetableptr, 0, sizeof( voltab_t ) );
		return true;
	} else {
		return false;
	}
}

void __far __pascal calcVolumeTable( bool sign )
{
	uint8_t vol;
	uint16_t sample;
	int16_t *voltab = ( int16_t * ) volumetableptr;

	if ( sign )
	{
		for ( vol = 0; vol <= 64; vol++ )
			for ( sample = 0; sample <= 255; sample++ )
			{
				*voltab = ( int16_t )( vol * ( int16_t )( ( int8_t )( sample ) ) ) >> 6;
				voltab++;
			}
	} else {
		for ( vol = 0; vol <= 64; vol++ )
			for ( sample = 0; sample <= 255; sample++ )
			{
				*voltab = ( int16_t )( vol * ( int16_t )( ( int8_t )( sample - 128 ) ) ) >> 6;
				voltab++;
			}
	}
}

void __far __pascal freeVolumeTable( void )
{
    if ( volumetableptr ) freedosmem( &volumetableptr );
}

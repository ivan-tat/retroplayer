/* printf.c -- Simplified printf() function definition.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#endif

#include "printf.h"

extern void __near __pascal printstring( char *s );

#ifdef __WATCOMC__
#pragma aux printstring modify [ ax bx cx dx si di ];
#endif

#define HEXDIGITS_LC( x ) ( (x) < 10 ? '0' + (x) : 'a' + (x) - 10 )
#define HEXDIGITS_UC( x ) ( (x) < 10 ? '0' + (x) : 'A' + (x) - 10 )

/* ------------------------------------------------------------------ */

void __near print_decimal (
    const unsigned long value,
    const bool          sign,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    unsigned long _value;
    unsigned char _count;
    char s[11];
    int i;

    i = 10;
    s[i] = 0;
    _value = value;
    _count = count <= 10 ? count : 10;
    do {
        if ( _count ) _count--;
        i--;
        if ( _value || i == 10 ) {
            s[i] = '0' + ( _value % 10 );
            _value /= 10;
        } else {
            s[i] = leadingzeroes ? '0' : ' ';
        }
    } while ( _value || _count );
    if ( sign ) {
        i--;
        s[i] = '-';
    }

    printstring( s + i );
}

/* ------------------------------------------------------------------ */

void __near print_decimal_signed(
    const signed long   value,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    unsigned long _value;
    bool sign;

    sign = value < 0;
    if ( sign ) _value = -value; else _value = value;

    print_decimal( _value, sign, leadingzeroes, count );
}

/* ------------------------------------------------------------------ */

void __near print_hexvalue (
    const unsigned long value,
    const bool          uppercase,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    unsigned long _value;
    unsigned char _count;
    char s[9];
    int i;

    i = 8;
    s[i] = 0;
    _value = value;
    _count = count <= 8 ? count : 8;
    do {
        if ( _count ) _count--;
        i--;
        if ( _value || i == 8 ) {
            s[i] = uppercase ? HEXDIGITS_UC( _value & 15 ) : HEXDIGITS_LC( _value & 15 );
            _value >>= 4;
        } else {
            s[i] = leadingzeroes ? '0' : ' ';
        }
    } while ( _value || _count );

    printstring( s + i );
}

/* ------------------------------------------------------------------ */

void __near print_uchar(
    const unsigned char value,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    print_decimal( value, false, leadingzeroes, count );
}

/* ------------------------------------------------------------------ */

void __near print_char(
    const signed char   value,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    print_decimal_signed( value, leadingzeroes, count );
}

/* ------------------------------------------------------------------ */

void __near print_uint(
    const unsigned int  value,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    print_decimal( value, false, leadingzeroes, count );
}

/* ------------------------------------------------------------------ */

void __near print_int(
    const signed int    value,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    print_decimal_signed( value, leadingzeroes, count );
}

/* ------------------------------------------------------------------ */

void __near print_ulong(
    const unsigned long value,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    print_decimal( value, false, leadingzeroes, count );
}

/* ------------------------------------------------------------------ */

void __near print_long(
    const signed long   value,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    print_decimal_signed( value, leadingzeroes, count );
}

/* ------------------------------------------------------------------ */

void __far printf( char *format, ... ) {
    va_list parminfo;
    int i, j;
    bool end;
    bool readnext;
    bool          f_long;
    bool          f_unsigned;
    bool          f_leadingzeroes;
    bool          f_uppercase;
    unsigned char f_count;

    va_start( parminfo, format );
    i = 0;
    j = 0;
    end = false;
    while ( ! end ) {
        switch ( format[ i ] ) {
        case '%':
            if ( i > j ) {
                format[ i ] = '\0';
                printstring( format + j );
                format[ i ] = '%';
            }
            i++;
            f_long = false;
            f_unsigned = false;
            f_leadingzeroes = false;
            f_uppercase = false;
            f_count = 0;

            do {
                readnext = false;
                switch ( format[ i ] ) {
                case '%':
                    printstring( "%" );
                    break;
                case '0':
                    if ( f_count == 0 ) {
                        f_leadingzeroes = true;
                        readnext = true;
                        break;
                    }
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    f_count = f_count * 10 + format[ i ] - '0';
                    readnext = true;
                    break;
                case 'c':
                case 'C':
                    if ( f_unsigned )
                        print_uchar( va_arg( parminfo, unsigned char ), f_leadingzeroes, f_count );
                    else
                        print_char( va_arg( parminfo, signed char ), f_leadingzeroes, f_count );
                    break;
                case 'd':
                case 'D':
                case 'i':
                case 'I':
                    if ( f_long ) {
                        if ( f_unsigned ) {
                            print_uint( va_arg( parminfo, unsigned long ), f_leadingzeroes, f_count );
                        } else
                            print_int( va_arg( parminfo, signed long ), f_leadingzeroes, f_count );
                    } else {
                        if ( f_unsigned ) {
                            print_uint( va_arg( parminfo, unsigned int ), f_leadingzeroes, f_count );
                        } else
                            print_int( va_arg( parminfo, signed int ), f_leadingzeroes, f_count );
                    }
                    break;
                case 'h':
                case 'H':
                        if ( f_unsigned ) {
                            print_uint( va_arg( parminfo, unsigned short ), f_leadingzeroes, f_count );
                        } else
                            print_int( va_arg( parminfo, signed short ), f_leadingzeroes, f_count );
                case 'l':
                case 'L':
                    f_long = true;
                    readnext = true;
                    break;
                case 's':
                case 'S':
                    printstring( va_arg( parminfo, char * ) );
                    break;
                case 'u':
                case 'U':
                    f_unsigned = true;
                    readnext = true;
                    break;
                case 'x':
                    if ( f_long )
                        print_hexvalue( va_arg( parminfo, unsigned long ), false, f_leadingzeroes, f_count );
                    else
                        print_hexvalue( va_arg( parminfo, unsigned int ), false, f_leadingzeroes, f_count );
                    break;
                case 'X':
                    if ( f_long )
                        print_hexvalue( va_arg( parminfo, unsigned long ), true, f_leadingzeroes, f_count );
                    else
                        print_hexvalue( va_arg( parminfo, unsigned int ), true, f_leadingzeroes, f_count );
                    break;
                case '\0':
                    end = true;
                    break;
                }
                i++;
                j = i;
            } while ( readnext );
            break;

        case '\0':
            if ( i - j ) printstring( format + j );
            end = true;
            break;

        default:
            i++;
        };
    };

    va_end( parminfo );
}

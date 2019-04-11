/* dos.h -- declarations for custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_DOS_H
#define CC_DOS_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdint.h>

#include "pascal.h"
#include "cc/dos/limits.h"
#include "cc/dos/int24.h"

/* DOS errors */

extern int _cc_doserrno;

struct CC_DOSERROR {
    int exterror;
    char errclass;
    char action;
    char locus;
};

int cc_dosexterr(struct CC_DOSERROR *err);

/* Console I/O */

void     _cc_dos_console_out(uint8_t c);
uint16_t _cc_dos_console_in(void);

struct cc_dosdate_t {
    unsigned char  day;         /* 1-31 */
    unsigned char  month;       /* 1-12 */
    unsigned short year;        /* 1980-2099 */
    unsigned char  dayofweek;   /* 0-6 (0=Sunday) */
};

struct cc_dostime_t {
    unsigned char hour;     /* 0-23 */
    unsigned char minute;   /* 0-59 */
    unsigned char second;   /* 0-59 */
    unsigned char hsecond;  /* 1/100 second; 0-99 */
};

void _cc_dos_getdate(struct cc_dosdate_t *d);
void _cc_dos_gettime(struct cc_dostime_t *t);

void __far *_cc_dos_getvect(unsigned num);
void __far _cc_dos_setvect(unsigned num, void __far *p);

#define __cc_dos_para(size) (((unsigned long)(size) + 15) >> 4)

#ifdef USE_INTRINSICS

#define _cc_dos_para(size) __cc_dos_para(size)

#else   /* !USE_INTRINSICS */

/*
unsigned _cc_dos_para(unsigned size);
*/
uint16_t __far _cc_dos_para (uint32_t size);

#endif  /* !USE_INTRINSICS */

/*
unsigned _cc_dos_allocmem(unsigned size, unsigned *seg);
unsigned _cc_dos_freemem(unsigned seg);
unsigned _cc_dos_setblock(unsigned size, unsigned seg, unsigned *max);
*/

// Pascal
uint16_t __far _cc_dos_allocmem (uint16_t size, uint16_t *seg);
uint16_t __far _cc_dos_freemem (uint16_t seg);
uint16_t __far _cc_dos_setblock (uint16_t size, uint16_t seg, uint16_t *max);

/* DOS Program Segment Prefix */

typedef struct cc_dospsp_t
{
    uint16_t int_20_opcode;
    uint16_t mem_size;
    uint8_t reserved1;
    uint8_t dos_func_dispatcher[5];
    void __far *int_22_ptr;
    void __far *int_23_ptr;
    void __far *int_24_ptr;
    uint16_t parent_seg;
    uint8_t file_handles_array[20];
    uint16_t env_seg;
    void __far *last_stack_ptr;
    uint16_t handles_array_size;
    void __far *handles_array_ptr;
    void __far *prev_psp_ptr;
    uint8_t reserved2[20];
    uint8_t int_21_retf_opcodes[3];
    uint8_t reserved3[9];
    uint8_t FCB1[16];
    uint8_t FCB2[20];
    uint8_t param_str[128];
};

/* DOS Memory Control Block */

typedef struct cc_dosmcb_t
{
    uint8_t ident;
    uint16_t owner_psp_seg;
    uint16_t size;
    uint8_t reserved[11];
    uint8_t program_name[8];
    uint8_t data;
};

unsigned _cc_dos_getpsp(void);
unsigned _cc_dos_getmasterpsp(void);

/* File's attributes (bit-field) */

#define _CC_A_NORMAL    0x00    // normal file
#define _CC_A_RDONLY    0x01    // read-only file
#define _CC_A_HIDDEN    0x02    // hidden file
#define _CC_A_SYSTEM    0x04    // system file
#define _CC_A_VOLID     0x08    // volume-ID entry
#define _CC_A_SUBDIR    0x10    // subdirectory
#define _CC_A_ARCH      0x20    // archive file

/* File mode (bit-field) */

#define CC_O_RDONLY_DOS     0       // open for read only
#define CC_O_WRONLY_DOS     1       // open for write only
#define CC_O_RDWR_DOS       2       // open for read and write
#define CC_O_NOINHERIT_DOS  0x80    // file is not inherited by child process (DOS only)

/* Symbolic constants for the _cc_dos_seek() function */

#define CC_SEEK_SET_DOS 0   // Seek relative to the start of file
#define CC_SEEK_CUR_DOS 1   // Seek relative to current position
#define CC_SEEK_END_DOS 2   // Seek relative to the end of the file

/*
unsigned _cc_dos_creat(const char *fname, unsigned attr, int *fd);
unsigned _cc_dos_creatnew(const char *fname, unsigned attr, int *fd);
unsigned _cc_dos_open(const char *fname, unsigned mode, int *fd);
unsigned _cc_dos_close(int fd);
unsigned _cc_dos_commit(int fd);
unsigned _cc_dos_read(int fd, void __far *buf, unsigned count, unsigned *numbytes);
unsigned _cc_dos_write(int fd, void __far *buf, unsigned count, unsigned *numbytes);
unsigned _cc_dos_seek(int fd, long offset, int kind, long *newoffset);
*/
uint16_t __far _cc_dos_creat (const char *fname, uint16_t attr, int16_t *fd);
uint16_t __far _cc_dos_creatnew (const char *fname, uint16_t attr, int16_t *fd);
uint16_t __far _cc_dos_open (const char *fname, uint16_t mode, int16_t *fd);
uint16_t __far _cc_dos_close (int16_t fd);
uint16_t __far _cc_dos_commit (int16_t fd);
uint16_t __far _cc_dos_read (int16_t fd, void __far *buf, uint16_t count, uint16_t *numbytes);
uint16_t __far _cc_dos_write (int16_t fd, void __far *buf, uint16_t count, uint16_t *numbytes);
uint16_t __far _cc_dos_seek (int16_t fd, int32_t offset, int16_t kind, int32_t *newoffset);

void _cc_dos_terminate(uint8_t code);

extern void __far __pascal pascal_swapvectors(void);
extern void __far __pascal pascal_exec(char *name, char *cmdline);

/*** Aliases ***/

#define DOSERROR CC_DOSERROR

#define dosexterr cc_dosexterr

#define _dos_console_out _cc_dos_console_out
#define _dos_console_in  _cc_dos_console_in

#define dosdate_t cc_dosdate_t
#define dostime_t cc_dostime_t

#define _dos_getdate  _cc_dos_getdate
#define _dos_gettime  _cc_dos_gettime

#define _dos_getvect _cc_dos_getvect
#define _dos_setvect _cc_dos_setvect

#define __dos_para __cc_dos_para
#define _dos_para _cc_dos_para

#define _dos_allocmem _cc_dos_allocmem
#define _dos_freemem  _cc_dos_freemem
#define _dos_setblock _cc_dos_setblock

#define dospsp_t cc_dospsp_t
#define dosmcb_t cc_dosmcb_t

#define _dos_getpsp       _cc_dos_getpsp
#define _dos_getmasterpsp _cc_dos_getmasterpsp

#define _A_NORMAL   _CC_A_NORMAL
#define _A_RDONLY   _CC_A_RDONLY
#define _A_HIDDEN   _CC_A_HIDDEN
#define _A_SYSTEM   _CC_A_SYSTEM
#define _A_VOLID    _CC_A_VOLID
#define _A_SUBDIR   _CC_A_SUBDIR
#define _A_ARCH     _CC_A_ARCH

#define O_RDONLY_DOS    CC_O_RDONLY_DOS
#define O_WRONLY_DOS    CC_O_WRONLY_DOS
#define O_RDWR_DOS      CC_O_RDWR_DOS
#define O_NOINHERIT_DOS CC_O_NOINHERIT_DOS

#define SEEK_SET_DOS    CC_SEEK_SET_DOS
#define SEEK_CUR_DOS    CC_SEEK_CUR_DOS
#define SEEK_END_DOS    CC_SEEK_END_DOS

#define _dos_creat      _cc_dos_creat
#define _dos_creatnew   _cc_dos_creatnew
#define _dos_open       _cc_dos_open
#define _dos_close      _cc_dos_close
#define _dos_commit     _cc_dos_commit
#define _dos_read       _cc_dos_read
#define _dos_write      _cc_dos_write
#define _dos_seek       _cc_dos_seek

#define _dos_terminate _cc_dos_terminate

/*** Linking ***/

#ifdef __WATCOMC__

/* Pascal >>> */
#pragma aux pascal_swapvectors "*" modify [ ax bx cx dx si di es ];
#pragma aux pascal_exec        "*" modify [ ax bx cx dx si di es ];
/* <<< Pascal */

#pragma aux _cc_doserrno "*";
#pragma aux cc_dosexterr "*";
#pragma aux _cc_dos_console_out "*";
#pragma aux _cc_dos_console_in "*";
#pragma aux _cc_dos_getdate "*";
#pragma aux _cc_dos_gettime "*";
#pragma aux _cc_dos_getvect "*";
#pragma aux _cc_dos_setvect "*";
#ifndef USE_INTRINSICS
#pragma aux _cc_dos_para "*";
#endif
#pragma aux _cc_dos_allocmem "*";
#pragma aux _cc_dos_freemem "*";
#pragma aux _cc_dos_setblock "*";
#pragma aux _cc_dos_getpsp "*";
#pragma aux _cc_dos_getmasterpsp "*";
#pragma aux _cc_dos_creat "*";
#pragma aux _cc_dos_creatnew "*";
#pragma aux _cc_dos_open "*";
#pragma aux _cc_dos_close "*";
#pragma aux _cc_dos_commit "*";
#pragma aux _cc_dos_read "*";
#pragma aux _cc_dos_write "*";
#pragma aux _cc_dos_seek "*";
#pragma aux _cc_dos_terminate "*";

#endif  /* __WATCOMC__ */

#endif  /* CC_DOS_H */

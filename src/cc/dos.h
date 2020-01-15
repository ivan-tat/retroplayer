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
    uint8_t FCB1[16];   /* see cc_dos_execparam_t structure */
    uint8_t FCB2[20];   /* see cc_dos_execparam_t structure */
    uint8_t param_str[128]; /* first byte is the length of arguments string */
        /* (not including terminating byte), then "length" bytes of a string */
        /* (126 bytes at most), and one byte 0x0d (CR) after the end of the string. */
};  /* 256 bytes */

/* DOS Memory Control Block */

typedef struct cc_dosmcb_t
{
    uint8_t ident;          /* 'M'=valid, 'Z'=last block in list */
    uint16_t owner_psp_seg; /* segment address of PSP of owner (0=owns self) */
    uint16_t size;          /* allocation size in paragraphs (16-bytes chunks) */
    uint8_t reserved[3];
    uint8_t owner_name[8];  /* NULL-terminated string, name of owner (DOS v4.0+) */
    /* data follows ("size"*16 bytes) */
};  /* 16 bytes */
/* After fn 4Bh Exec, a 'Z' block is stored at (PSP-1):0 of the new process. */

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

/* IOCTL */

/* IOCTL file/device information record (bit-field)
 *
 * For file:
 *   0x003f - drive ID (0=A, 1=B, etc.)
 *   0x0040 - 0=EOF on input; 1=not EOF
 *   0x0080 - 0=file
 *   0x0700 - 0
 *   0x0800 - supports device open and close, removable media (0x0d, 0x0e, 0x0f)
 *   0x1000 - 0
 *   0x2000 - 0=needs FAT with BIOS parameter block (0x02)
 *   0x4000 - supports IOCTL read, write (0x03, 0x0c)
 *   0x8000 - 0=block device; 1=char device
 *
 * For device:
 *   0x0001 - console input device
 *   0x0002 - console output device
 *   0x0004 - null device
 *   0x0008 - clock device
 *   0x0010 - special device; fast output
 *   0x0020 - 0=ASCII mode; 1=binary mode
 *   0x0040 - 0=EOF on input; 1=not EOF
 *   0x0080 - 1=device
 *   0x0700 - 0
 *   0x0800 - supports device open, close (0x0d, 0x0e)
 *   0x1000 - 0
 *   0x2000 - supports output till busy (0x10)
 *   0x4000 - supports IOCTL read, write (0x03, 0x0c)
 *   0x8000 - 0=block device; 1=char device
 */

typedef uint16_t cc_ioctl_info_t;

/* File Control Block (FCB) */

#pragma pack(push, 1);
typedef struct cc_dosfcb_t
{
    uint8_t drive;      /* Drive ID */
                        /* Before open: 0=default, 1=A, 2=B, etc. */
                        /* After open:  0=A, 1=B, etc. */
    uint8_t name [8];   /* File name, left-justified, padded with spaces if less then 8 characters */
    uint8_t ext [3];    /* File extension, left-justified, padded with spaces if less then 3 characters */
    uint16_t cur_blk;   /* Current block number (a block is 128 records) */
                        /* DOS sets to 0 on open. */
    uint16_t rec_size;  /* Logical record size (in bytes) */
                        /* DOS sets to 128 on open. */
    uint32_t file_size; /* Length of file (in bytes) */
    uint16_t date;      /* Date created/last modified in packed format */
    uint16_t time;      /* Time created/last modified in packed format */
    uint8_t resv [8];   /* Reserved */
    uint8_t cur_rec;    /* Current position in current block (0-127) */
    uint32_t rand_rec;  /* Current record number in entire file */
};  /* 37 bytes */
#pragma pack(pop);

#pragma pack(push, 1);
typedef struct cc_dos_execparam_t
{
    uint16_t env_seg;           /* segment of environment for child or 0 (current) */
    const char __far *cmd_tail; /* text to be placed to [PSP:0x80] */
    struct cc_dosfcb_t __far *fcb[2];
        /* address of FCB to be placed at [PSP:0x5c] and [PSP:0x6c] */
};  /* 14 bytes */
#pragma pack(pop);

/*
unsigned _cc_dos_creat(const char *fname, unsigned attr, int *fd);
unsigned _cc_dos_creatnew(const char *fname, unsigned attr, int *fd);
unsigned _cc_dos_open(const char *fname, unsigned mode, int *fd);
unsigned _cc_dos_close(int fd);
unsigned _cc_dos_commit(int fd);
unsigned _cc_dos_read(int fd, void __far *buf, unsigned count, unsigned *numbytes);
unsigned _cc_dos_write(int fd, void __far *buf, unsigned count, unsigned *numbytes);
unsigned _cc_dos_seek(int fd, long offset, int kind, long *newoffset);

unsigned _cc_dos_ioctl_query_flags (int16_t fd, cc_ioctl_info_t *info);
*/
uint16_t __far _cc_dos_creat (const char *fname, uint16_t attr, int16_t *fd);
uint16_t __far _cc_dos_creatnew (const char *fname, uint16_t attr, int16_t *fd);
uint16_t __far _cc_dos_open (const char *fname, uint16_t mode, int16_t *fd);
uint16_t __far _cc_dos_close (int16_t fd);
uint16_t __far _cc_dos_commit (int16_t fd);
uint16_t __far _cc_dos_read (int16_t fd, void __far *buf, uint16_t count, uint16_t *numbytes);
uint16_t __far _cc_dos_write (int16_t fd, void __far *buf, uint16_t count, uint16_t *numbytes);
uint16_t __far _cc_dos_seek (int16_t fd, int32_t offset, int16_t kind, int32_t *newoffset);

uint16_t __far _cc_dos_ioctl_query_flags (int16_t fd, cc_ioctl_info_t __far *info);

/* "option" for _cc_dos_parsfnm() (bit-field): */

#define CC_FCB_RMPATH   (1 << 0)
#define CC_FCB_SETDRV   (1 << 1)
#define CC_FCB_SETNAME  (1 << 2)
#define CC_FCB_SETEXT   (1 << 3)

char *_cc_dos_parsfnm (const char *fname, struct cc_dosfcb_t *fcb, char option);

/* Context switching */

// Saved critical interrupt vectors
#define SAVEINTVEC_COUNT 19
extern const uint8_t SaveIntVecIndexes[SAVEINTVEC_COUNT];
extern void __far *SaveIntVecs[SAVEINTVEC_COUNT];

void cc_dos_savevectors (void);
void cc_dos_restorevectors (void);
void cc_dos_swapvectors (void);
unsigned _cc_dos_exec (uint16_t env_seg, const char *fname, const char *cmd);
void _cc_dos_terminate(uint8_t code);

//#if LINKER_TPC == 1
extern void __far __pascal pascal_swapvectors(void);
extern void __far __pascal pascal_exec(char *name, char *cmdline);
//#endif  /* LINKER_TPC == 1 */

/*** Aliases ***/

#define DOSERROR CC_DOSERROR

#define _doserrno _cc_doserrno
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

#define ioctl_info_t cc_ioctl_info_t

#define _dos_creat      _cc_dos_creat
#define _dos_creatnew   _cc_dos_creatnew
#define _dos_open       _cc_dos_open
#define _dos_close      _cc_dos_close
#define _dos_commit     _cc_dos_commit
#define _dos_read       _cc_dos_read
#define _dos_write      _cc_dos_write
#define _dos_seek       _cc_dos_seek

#define _dos_ioctl_query_flags _cc_dos_ioctl_query_flags

#define FCB_RMPATH  CC_FCB_RMPATH
#define FCB_SETDRV  CC_FCB_SETDRV
#define FCB_SETNAME CC_FCB_SETNAME
#define FCB_SETEXT  CC_FCB_SETEXT

#define _dos_parsfnm _cc_dos_parsfnm

#define dos_savevectors cc_dos_savevectors
#define dos_restorevectors cc_dos_restorevectors
#define dos_swapvectors cc_dos_swapvectors
#define _dos_exec _cc_dos_exec
#define _dos_terminate _cc_dos_terminate

/*** Linking ***/

#ifdef __WATCOMC__

//#if LINKER_TPC == 1
#pragma aux pascal_swapvectors "*" modify [ ax bx cx dx si di es ];
#pragma aux pascal_exec        "*" modify [ ax bx cx dx si di es ];
//#endif  /* LINKER_TPC == 1 */

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
#pragma aux _cc_dos_ioctl_query_flags "*";
#pragma aux _cc_dos_parsfnm "*";
#pragma aux _cc_dos_exec "*";
#pragma aux SaveIntVecIndexes "*";
#pragma aux SaveIntVecs "*";
#pragma aux cc_dos_savevectors "*";
#pragma aux cc_dos_restorevectors "*";
#pragma aux cc_dos_swapvectors "*";
#pragma aux _cc_dos_terminate "*";

#endif  /* __WATCOMC__ */

#endif  /* CC_DOS_H */

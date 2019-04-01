(* fillvars.pas -- Pascal declarations for fillvars.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit fillvars;

interface

uses
    dma,
    sndctl_t;

(*$I defines.pas*)

type
    TSNDDMABUFFLAGS = Word;

const
    SNDDMABUFFL_LOCKED = 1 shl 0;
    SNDDMABUFFL_SLOW   = 1 shl 1;
    SNDDMABUFFL_LQ     = 1 shl 2;

const
    DMA_BUF_SIZE_MAX = 8*1024;

type
    TSNDDMABUF = packed record
        flags: TSNDDMABUFFLAGS;
        buf: PDMABUF;
        format: THWSMPFMT;
        frameSize: word;
        framesCount: byte;
        frameLast: shortint;
        frameActive: byte;
    end;
    PSNDDMABUF = ^TSNDDMABUF;

var
    sndDMABuf: TSNDDMABUF;

procedure snddmabuf_init;
procedure snddmabuf_alloc;
procedure snddmabuf_get_frame_offset;
procedure snddmabuf_get_frame;
procedure snddmabuf_get_offset_from_count;
procedure snddmabuf_get_count_from_offset;
procedure snddmabuf_free;

implementation

uses
    strutils,
    dos_,
    stdio,
    string_,
    debug,
    common,
    sbctl,
    s3mvars;

(*$l fillvars.obj*)

procedure snddmabuf_init; external;
procedure snddmabuf_alloc; external;
procedure snddmabuf_get_frame_offset; external;
procedure snddmabuf_get_frame; external;
procedure snddmabuf_get_offset_from_count; external;
procedure snddmabuf_get_count_from_offset; external;
procedure snddmabuf_free; external;

end.

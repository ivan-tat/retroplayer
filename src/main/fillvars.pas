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

const
    DMA_BUF_SIZE_MAX = 8*1024;

type
    TSNDDMABUF = packed record
        buf: PDMABUF;
        format: THWSMPFMT;
        frameSize: word;
        framesCount: byte;
        frameLast: byte;
        frameActive: byte;
        flags_locked: boolean;
        flags_Slow: boolean;
    end;
    PSNDDMABUF = ^TSNDDMABUF;

var
    sndDMABuf: TSNDDMABUF;
    playOption_FPS: byte;
    playOption_LowQuality: boolean;

procedure snddmabuf_init(self: PSNDDMABUF);
function  snddmabuf_alloc(self: PSNDDMABUF; size: longint): boolean;
function  snddmabuf_get_frame_offset(self: PSNDDMABUF; index: byte): word;
function  snddmabuf_get_frame(self: PSNDDMABUF; index: byte): pointer;
function  snddmabuf_get_offset_from_count(self: PSNDDMABUF; value: word): word;
function  snddmabuf_get_count_from_offset(self: PSNDDMABUF; value: word): word;
procedure snddmabuf_free(self: PSNDDMABUF);

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

procedure snddmabuf_init(self: PSNDDMABUF); external;
function  snddmabuf_alloc(self: PSNDDMABUF; size: longint): boolean; external;
function  snddmabuf_get_frame_offset(self: PSNDDMABUF; index: byte): word; external;
function  snddmabuf_get_frame(self: PSNDDMABUF; index: byte): pointer; external;
function  snddmabuf_get_offset_from_count(self: PSNDDMABUF; value: word): word; external;
function  snddmabuf_get_count_from_offset(self: PSNDDMABUF; value: word): word; external;
procedure snddmabuf_free(self: PSNDDMABUF); external;

end.

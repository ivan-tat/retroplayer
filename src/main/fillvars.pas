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
    TSNDDMABUF = record
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

function  sndDMABufGetFrameOff(buf: PSNDDMABUF; index: byte): word;
function  sndDMABufGetOffFromCount(buf: PSNDDMABUF; count: word): word;
function  sndDMABufGetCountFromOff(buf: PSNDDMABUF; off: word): word;
function  sndDMABufAlloc(buf: PSNDDMABUF; size: longint): boolean;
procedure sndDMABufFree(buf: PSNDDMABUF);

procedure sndDMABufInit(buf: PSNDDMABUF);
procedure sndDMABufDone(buf: PSNDDMABUF);

implementation

uses
    dos_,
    stdio,
    string_,
    strutils,
    sbctl,
    s3mvars;

(*$l fillvars.obj*)

function  sndDMABufGetFrameOff(buf: PSNDDMABUF; index: byte): word; external;
function  sndDMABufGetOffFromCount(buf: PSNDDMABUF; count: word): word; external;
function  sndDMABufGetCountFromOff(buf: PSNDDMABUF; off: word): word; external;
function  sndDMABufAlloc(buf: PSNDDMABUF; size: longint): boolean; external;
procedure sndDMABufFree(buf: PSNDDMABUF); external;

procedure sndDMABufInit(buf: PSNDDMABUF); external;
procedure sndDMABufDone(buf: PSNDDMABUF); external;

end.

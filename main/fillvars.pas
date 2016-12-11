(* fillvars.pas -- Pascal declarations for fillvars.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit fillvars;

interface

const
    DMA_BUF_SIZE_MAX = 8*1024;

(* DMA buffer *)
var
    DMABufUnaligned: pointer;
    DMABuf: pointer;
    DMABufSize: longint;
    DMABufFrameSize: word;
    DMABufFramesCount: byte;
    DMABufFrameLast: byte;
    DMABufFrameActive: byte;
    DMAFlags_JustInFill: boolean;
    DMAFlags_Slow: boolean;

(* player *)
var
    playOption_FPS: byte;
    playOption_LowQuality: boolean;

(* EMM *)
var
    SavHandle: word;

function  getDMABufFrameOff(index: byte): word;
function  getDMABufOffFromCount(count: word): word;
function  getCountFromDMABufOff(bufOff: word): word;
procedure initDMABuf;
function  allocDMABuf(dmaSize: longint): boolean;
procedure freeDMABuf;
procedure doneDMABuf;

implementation

uses
    memset,
    strutils,
    printf,
    dosproc,
    sbctl;

(*$l fillvars.obj*)

function  getDMABufFrameOff(index: byte): word; external;
function  getDMABufOffFromCount(count: word): word; external;
function  getCountFromDMABufOff(bufOff: word): word; external;
procedure initDMABuf; external;
function  allocDMABuf(dmaSize: longint): boolean; external;
procedure freeDMABuf; external;
procedure doneDMABuf; external;

end.

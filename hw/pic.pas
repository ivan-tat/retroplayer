(* pic.pas -- declarations for pic.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit pic;

interface

(* pic.c *)

(* Hardware I/O *)

type
    TIRQMask = word;

procedure picEnableChannels(mask: TIRQMask);
procedure picDisableChannels(mask: TIRQMask);
procedure picEOI(irq: byte);
function  picGetISR(irq: byte): pointer;
procedure picSetISR(irq: byte; p: pointer);

(* Sharing IRQ channels *)

type
  TISRCallback = procedure (ch: byte);
  PISRCallback = ^TISRCallback;

procedure isrHookSingleChannel(ch: byte);
procedure isrReleaseSingleChannel(ch: byte);
procedure isrSetSingleChannelHandler(ch: byte; p: PISRCallback);
function  isrGetSingleChannelHandler(ch: byte): PISRCallback;
procedure isrClearSingleChannelHandler(ch: byte);

(* Initialization *)

procedure isrInit;
procedure isrDone;

implementation

uses
    dos;

(*$l isr.obj*)
(*$l pic.obj*)

(* Hardware I/O *)

(* pic.c *)
procedure picEnableChannels(mask: TIRQMask); external;
procedure picDisableChannels(mask: TIRQMask); external;
procedure picEOI(irq: byte); external;
function  picGetISR(irq: byte): pointer; external;
procedure picSetISR(irq: byte; p: pointer); external;

(* Sharing IRQ channels *)

(* isr_.asm *)
function getISR(ch: byte): pointer; external;

(* pic.c *)
procedure isrHookSingleChannel(ch: byte); external;
procedure isrReleaseSingleChannel(ch: byte); external;
procedure isrSetSingleChannelHandler(ch: byte; p: PISRCallback); external;
function  isrGetSingleChannelHandler(ch: byte): PISRCallback; external;
procedure isrClearSingleChannelHandler(ch: byte); external;

(* Initialization *)

(* pic.c *)
procedure isrInit; external;
procedure isrDone; external;

end.

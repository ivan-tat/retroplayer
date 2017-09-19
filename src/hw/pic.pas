(* pic.pas -- declarations for pic.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit pic;

interface

(*$I defines.pas*)

type
    TIRQMask = word;

procedure picEnableChannels(mask: TIRQMask);
procedure picDisableChannels(mask: TIRQMask);
procedure picEOI(irq: byte);
function  picGetISR(irq: byte): pointer;
procedure picSetISR(irq: byte; p: pointer);

type
  TISRCallback = procedure (ch: byte);
  PISRCallback = ^TISRCallback;

procedure isrHookSingleChannel(ch: byte);
procedure isrReleaseSingleChannel(ch: byte);
procedure isrSetSingleChannelHandler(ch: byte; p: PISRCallback);
function  isrGetSingleChannelHandler(ch: byte): PISRCallback;
procedure isrClearSingleChannelHandler(ch: byte);

implementation

uses
    pascal,
    stdio;

(*$l isr.obj*)

function getISR(ch: byte): pointer; external;

(*$l pic.obj*)

procedure picEnableChannels(mask: TIRQMask); external;
procedure picDisableChannels(mask: TIRQMask); external;
procedure picEOI(irq: byte); external;
function  picGetISR(irq: byte): pointer; external;
procedure picSetISR(irq: byte; p: pointer); external;

procedure isrHookSingleChannel(ch: byte); external;
procedure isrReleaseSingleChannel(ch: byte); external;
procedure isrSetSingleChannelHandler(ch: byte; p: PISRCallback); external;
function  isrGetSingleChannelHandler(ch: byte): PISRCallback; external;
procedure isrClearSingleChannelHandler(ch: byte); external;

procedure register_pic; far; external;
procedure unregister_pic; far; external;

begin
    register_pic;
end.

(* detisr.pas -- Pascal declarations for detisr.asm/detisr_.asm.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit detisr;

interface

type
  TDetISRCallback = procedure ( irq: byte );
  PDetISRCallback = ^TDetISRCallback;

function GetDetISR( irq: byte ): pointer;
procedure SetDetISRCallback( p: PDetISRCallback );

implementation

var
    DetISRCallback: PDetISRCallback;

(*$l detisr_.obj*)
(*$l detisr.obj*)

function GetDetISR( irq: byte ): pointer; external;
procedure SetDetISRCallback( p: PDetISRCallback ); external;

end.

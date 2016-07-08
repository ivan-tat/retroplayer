(* Declarations for PIC.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit pic;

interface

procedure picEnableIRQs( mask: word );
procedure picDisableIRQs( mask: word );
procedure picEOI( irq: byte );
function picGetIntVec( irq: byte ): pointer;
procedure picSetIntVec( irq: byte; p: pointer );

implementation

uses dos;

(*$l pic.obj*)

procedure picEnableIRQs( mask: word ); external;
procedure picDisableIRQs( mask: word ); external;
procedure picEOI( irq: byte ); external;
function picGetIntVec( irq: byte ): pointer; external;
procedure picSetIntVec( irq: byte; p: pointer ); external;

end.

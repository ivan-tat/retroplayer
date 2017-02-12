(* dos_.pas -- Pascal declarations for dos_.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit dos_;

interface

procedure _dos_allocmem_;
procedure _dos_freemem_;
procedure _dos_setblock_;

implementation

uses
    pascal,
    i86;

(*$l dos_.obj*)
procedure _dos_allocmem_; external;
procedure _dos_freemem_; external;
procedure _dos_setblock_; external;

end.

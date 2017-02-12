(* dos_.pas -- Pascal declarations for dos_.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit dos_;

interface

function _dos_allocmem(size: word; var seg: word): word;
function _dos_freemem(seg: word): word;
function _dos_setblock(size: word; seg: word; var max: word): word;

implementation

uses
    pascal,
    i86;

(*$l dos_.obj*)
function _dos_allocmem(size: word; var seg: word): word; external;
function _dos_freemem(seg: word): word; external;
function _dos_setblock(size: word; seg: word; var max: word): word; external;

end.

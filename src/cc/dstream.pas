(* dstream.pas -- Pascal declarations for "dstream" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit dstream;

interface

(*$I defines.pas*)

procedure datastream_init;
procedure datastream_write;
procedure datastream_flush;

implementation

uses
    watcom,
    pascal,
    string_;

(*$L dstream.obj*)

procedure datastream_init; external;
procedure datastream_write; external;
procedure datastream_flush; external;

end.

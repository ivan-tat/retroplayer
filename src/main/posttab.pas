(* posttab.pas -- Pascal declarations for posttab.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit posttab;

interface

(*$I defines.pas*)

procedure calcPostTable( vol: byte; use16bit: boolean );
procedure convert_16s_8u;
procedure convert_16s_mono_8u_mono_lq;
procedure convert_16s_stereo_8u_stereo_lq;

implementation

uses
    watcom;

(*$l posttab.obj*)

procedure calcPostTable( vol: byte; use16bit: boolean ); external;
procedure convert_16s_8u; external;
procedure convert_16s_mono_8u_mono_lq; external;
procedure convert_16s_stereo_8u_stereo_lq; external;

end.

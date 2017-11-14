(* posttab.pas -- Pascal declarations for posttab.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit posttab;

interface

(*$I defines.pas*)

procedure calcPostTable(volume: Byte);
procedure amplify_16s;
procedure clip_16s_8u;
procedure clip_16s_mono_8u_mono_lq;
procedure clip_16s_stereo_8u_stereo_lq;

implementation

uses
    watcom;

(*$l posttab.obj*)

procedure calcPostTable(volume: Byte); external;
procedure amplify_16s; external;
procedure clip_16s_8u; external;
procedure clip_16s_mono_8u_mono_lq; external;
procedure clip_16s_stereo_8u_stereo_lq; external;

end.

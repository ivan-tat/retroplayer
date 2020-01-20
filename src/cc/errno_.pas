(* errno.pas -- Pascal declarations for custom "errno" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    errno_;

interface

(*$I defines.pas*)

(*$ifdef DEFINE_LOCAL_DATA*)

var
    cc_errno: Integer;

(*$endif*)  (* DEFINE_LOCAL_DATA *)

implementation

(*$L errno/errno.obj*)

end.

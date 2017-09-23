(* errno.pas -- Pascal declarations for custom "errno" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    errno_;

interface

(*$I defines.pas*)

var
    _cc_errno: Integer;

(* Alias *)

var
    _errno: Integer absolute _cc_errno;

implementation

(*$L errno/errno.obj*)

end.
(* string_.pas -- Pascal declarations for C string library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit string_;

interface

procedure memcmp_;
procedure memcpy_;
procedure memset_;
procedure strlen_;

implementation

(*$l memcmp.obj*)
procedure memcmp_; external;

(*$l memcpy.obj*)
procedure memcpy_; external;

(*$l memset.obj*)
procedure memset_; external;

(*$l strlen.obj*)
procedure strlen_; external;

end.

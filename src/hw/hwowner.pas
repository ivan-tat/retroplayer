(* hwowner.pas -- Declarations for hwowner.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit hwowner;

interface

(*$I defines.pas*)

procedure hwowner_register;
procedure hwowner_get_id;
procedure hwowner_get_name;
procedure hwowner_unregister;

procedure register_hwowner;

implementation

uses
    pascal,
    i86,
    dos_,
    stdlib,
    string_,
    common,
    debug;

(*$l hwowner.obj*)

procedure hwowner_register; external;
procedure hwowner_get_id; external;
procedure hwowner_get_name; external;
procedure hwowner_unregister; external;

procedure register_hwowner; external;

end.

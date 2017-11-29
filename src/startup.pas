(* startup.pas -- Pascal declarations for "startup" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    startup;

interface

(*$I defines.pas*)

function  pascal_paramcount: Word;
procedure pascal_paramstr(var dest: String; i: Byte);

function  custom_argc: Word;
procedure custom_argv(dest: PChar; n: Word; i: Byte);

implementation

uses
    pascal;

function pascal_paramcount: Word;
begin
    pascal_paramcount := System.ParamCount;
end;

procedure pascal_paramstr(var dest: String; i: Byte);
begin
    dest := System.ParamStr(i);
end;

(*$l startup.obj*)

function  custom_argc: Word; external;
procedure custom_argv(dest: PChar; n: Word; i: Byte); external;

end.

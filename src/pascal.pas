(* pascal.pas -- support for Pascal linker and Pascal units wrapper.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit pascal;

interface

(*$I defines.pas*)

procedure pascal_fillchar(var dest; size: Word; value: Byte);
procedure pascal_move(var src, dest; size: Word);

procedure strpastoc(dest: PChar; var src: String; maxlen: Word);
procedure strctopas(var dest: String; src: PChar; maxlen: Word);
{
procedure strpastoc(dest, src: Pointer; maxlen: Word);
procedure strctopas(dest, src: Pointer; maxlen: Byte);
}

implementation

procedure pascal_fillchar(var dest; size: Word; value: Byte);
begin
    System.FillChar(dest, size, value);
end;

procedure pascal_move(var src, dest; size: Word);
begin
    System.Move(src, dest, size);
end;

(*$l pascal.obj*)

procedure strpastoc(dest: PChar; var src: String; maxlen: Word); external;
procedure strctopas(var dest: String; src: PChar; maxlen: Word); external;

end.

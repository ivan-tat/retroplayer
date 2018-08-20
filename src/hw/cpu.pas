(* cpu.pas -- Declarations for cpu.asm.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit cpu;

interface

(*$I defines.pas*)

procedure isCPU_8086;
procedure isCPU_i386;

implementation

(*$l cpu.obj*)

procedure isCPU_8086; external;
procedure isCPU_i386; external;

end.

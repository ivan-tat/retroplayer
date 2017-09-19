(* cpu.pas -- Declarations for cpu.asm.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit cpu;

interface

function  isCPU_i386: boolean;

implementation

(*$l cpu.obj*)
function  isCPU_i386: boolean; external;

end.

(* voltab.pas -- Pascal declarations for voltab.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit voltab;

interface

(*$I defines.pas*)

type
    voltab_t = array[0..64,0..255] of integer;
    voltab_p = ^voltab_t;

var
    VolumeTablePtr: voltab_p;

procedure initVolumeTable;
function  allocVolumeTable: boolean;
procedure calcVolumeTable;
procedure freeVolumeTable;

implementation

uses
    string_,
    dos_;

(*$l voltab.obj*)
procedure initVolumeTable; external;
function  allocVolumeTable: boolean; external;
procedure calcVolumeTable; external;
procedure freeVolumeTable; external;

end.

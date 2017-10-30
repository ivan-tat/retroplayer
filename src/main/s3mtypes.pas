(* s3mtypes.pas -- Pascal declarations for s3mtypes.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit s3mtypes;

interface

uses types;

(*$I defines.pas*)

(* generic module information *)

const
    MOD_MAX_TITLE_LENGTH = 28;

type
    TModTitle = string [MOD_MAX_TITLE_LENGTH];
    PModTitle = ^TModTitle;

const
    MOD_MAX_TRACKER_NAME_LENGTH = 31;

type
    TModTrackerName = string [MOD_MAX_TRACKER_NAME_LENGTH];
    PModTrackerName = ^TModTrackerName;

const
    MAX_ORDERS = 255; (* 0..254 orders *)

(* song arrangement *)

type
    TOrderEntry = byte;

type
    TOrderArray = array [0..MAX_ORDERS] of TOrderEntry;
        (* song arrangement *)

implementation

end.

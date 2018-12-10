(* ctype.pas -- Pascal declarations for custom "ctype" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit ctype;

interface

(*$I defines.pas*)

const
    _CC_CNTRL  = $01;
    _CC_SPACE  = $02;
    _CC_PUNCT  = $04;
    _CC_PRINT  = $08;
    _CC_XDIGT  = $10;
    _CC_DIGIT  = $20;
    _CC_UPPER  = $40;
    _CC_LOWER  = $80;

const _cc_IsTable: array [0..256] of byte =
(
    0,

    (* 0x00 - 0x0f *)
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL or _CC_SPACE,
    _CC_CNTRL or _CC_SPACE,
    _CC_CNTRL or _CC_SPACE,
    _CC_CNTRL or _CC_SPACE,
    _CC_CNTRL or _CC_SPACE,
    _CC_CNTRL,
    _CC_CNTRL,

    (* 0x10 - 0x1f *)
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,
    _CC_CNTRL,

    (* 0x20 - 0x2f *)
    _CC_PRINT or _CC_SPACE,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,

    (* 0x30 - 0x3f *)
    _CC_PRINT or _CC_DIGIT or _CC_XDIGT,
    _CC_PRINT or _CC_DIGIT or _CC_XDIGT,
    _CC_PRINT or _CC_DIGIT or _CC_XDIGT,
    _CC_PRINT or _CC_DIGIT or _CC_XDIGT,
    _CC_PRINT or _CC_DIGIT or _CC_XDIGT,
    _CC_PRINT or _CC_DIGIT or _CC_XDIGT,
    _CC_PRINT or _CC_DIGIT or _CC_XDIGT,
    _CC_PRINT or _CC_DIGIT or _CC_XDIGT,
    _CC_PRINT or _CC_DIGIT or _CC_XDIGT,
    _CC_PRINT or _CC_DIGIT or _CC_XDIGT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,

    (* 0x40 - 0x4f *)
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_UPPER or _CC_XDIGT,
    _CC_PRINT or _CC_UPPER or _CC_XDIGT,
    _CC_PRINT or _CC_UPPER or _CC_XDIGT,
    _CC_PRINT or _CC_UPPER or _CC_XDIGT,
    _CC_PRINT or _CC_UPPER or _CC_XDIGT,
    _CC_PRINT or _CC_UPPER or _CC_XDIGT,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,

    (* 0x50 - 0x5f *)
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_UPPER,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,

    (* 0x60 - 0x6f *)
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_LOWER or _CC_XDIGT,
    _CC_PRINT or _CC_LOWER or _CC_XDIGT,
    _CC_PRINT or _CC_LOWER or _CC_XDIGT,
    _CC_PRINT or _CC_LOWER or _CC_XDIGT,
    _CC_PRINT or _CC_LOWER or _CC_XDIGT,
    _CC_PRINT or _CC_LOWER or _CC_XDIGT,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,

    (* 0x70 - 0x7f *)
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_LOWER,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_PRINT or _CC_PUNCT,
    _CC_CNTRL,

    (* 0x80 - 0xff *)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
);

(* Aliases *)

const
    _CNTRL  = _CC_CNTRL;
    _SPACE  = _CC_SPACE;
    _PUNCT  = _CC_PUNCT;
    _PRINT  = _CC_PRINT;
    _XDIGT  = _CC_XDIGT;
    _DIGIT  = _CC_DIGIT;
    _UPPER  = _CC_UPPER;
    _LOWER  = _CC_LOWER;

var
    __IsTable: array [0..256] of byte absolute _cc_IsTable;

implementation

end.

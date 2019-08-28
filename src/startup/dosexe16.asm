; dosexe16.asm -- part of custom "startup" library.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.387
.model large

include "startup.def"

extern _start_c: far proc

CONST segment word public use16 'DATA'
CONST ends

CONST2 segment word public use16 'DATA'
CONST2 ends

_DATA segment word public use16 'DATA'
_DATA ends

_BSS segment word public use16 'BSS'
_BSS ends

STACK segment word public use16 'STACK'
STACK ends

DGROUP group CONST, CONST2, _DATA, _BSS, STACK

dosexe16_TEXT segment word public use16 'CODE'
assume cs:dosexe16_TEXT, ds:dgroup, ss:dgroup

public _start_asm
_start_asm:
    mov     ax, DGROUP
    mov     ds, ax
    mov     DGROUP:[_cc_psp], es
    jmp     _start_c

dosexe16_TEXT ends

end

; ints.asm -- part of custom "conio" library.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large

include "cc/conio.def"

_DATA segment word public use16 'DATA'
_DATA ends

DGROUP group _DATA

INTS_TEXT segment word public use16 'CODE'
assume cs:INTS_TEXT,ds:DGROUP

public _cc_local_int1b_asm
_cc_local_int1b_asm:
    push    ax
    push    ds
    mov     ax,DGROUP:_DATA
    mov     ds,ax
    cmp     cc_checkbreak,0
    jz      short @no_break
    mov     cc_gotbreak,1
@no_break:
    pop     ds
    pop     ax
    iret

INTS_TEXT ends

end

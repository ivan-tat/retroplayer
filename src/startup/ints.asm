; ints.asm -- part of custom "startup" library.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large

include "startup.def"

_DATA segment word public use16 'DATA'
_DATA ends

DGROUP group _DATA

INTS_TEXT segment word public use16 'CODE'
assume cs:INTS_TEXT,ds:DGROUP

public _cc_local_int0_asm
_cc_local_int0_asm:
    ; Stack:
    ;   SS:[SP+0] (dword) = void __far *addr
    ;   SS:[SP+4] (word)  = int flags
    mov     ax,DGROUP:_DATA
    mov     ds,ax
    call    far ptr _cc_local_int0

public _cc_local_int23_asm
; To abort application raise carry flag and perform a far return via "RETF".
_cc_local_int23_asm:
    ; Stack:
    ;   SS:[SP+0] (dword) = void __far *addr
    ;   SS:[SP+4] (word)  = int flags
    mov     ax,DGROUP:_DATA
    mov     ds,ax
    call    far ptr _cc_local_int23

INTS_TEXT ends

end

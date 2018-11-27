; ints.asm -- part of custom "startup" library.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large

_DATA segment word public use16 'DATA'
_DATA ends

DGROUP group _DATA

INTS_TEXT segment word public use16 'CODE'
assume cs:INTS_TEXT,ds:DGROUP

extrn _cc_local_int0: far
public _cc_local_int0_asm
; Stack:
;   SP+0:   IP, CS, FLAGS - to get back to application via IRET
_cc_local_int0_asm:
    mov     ax,DGROUP:_DATA
    mov     ds,ax
    call    far ptr _cc_local_int0

extrn _cc_local_int23: far
public _cc_local_int23_asm
; To abort application raise carry flag and perform a far return via "RETF".
; Stack:
;   SP+0:   IP, CS, FLAGS - to get back to application via IRET;
_cc_local_int23_asm:
    mov     ax,DGROUP:_DATA
    mov     ds,ax
    call    far ptr _cc_local_int23

INTS_TEXT ends

end

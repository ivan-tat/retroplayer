; int24.asm -- part of custom "dos" library.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large

_DATA segment word public use16 'DATA'
_DATA ends

DGROUP group _DATA

INT24_TEXT segment word public use16 'CODE'
assume cs:INT24_TEXT,ds:DGROUP

public _cc_local_int24_asm
_cc_local_int24_asm:
; In:
;   BP:SI = device header
;   DI = bits 0-7 are error code
;   AL = drive number (0=A, 1=B, etc.) if bit 7 of AH is cleared
;   AH = error information
; Stack:
;   SP+0:   IP, CS, FLAGS - to get back to DOS via IRET
;   SP+6:   AX, BX, CX, DX, SI, DI, BP, DS, ES - application program registers before INT 0x21
;   SP+18h: IP, CS, FLAGS - to get back to application via IRET

        sti
        add     sp,6    ; remove return address to DOS via IRET
        pop     ax      ; get saved DOS' function number, remove it from stack
        and     di,1fh
        add     di,96h
        cmp     ah,39h
        jae     short @skip
        mov     di,-1
@skip:                  ; DI now holds resulting error code for application
                        ; which will be returned in AX
        push    di      ; save it back in stack
                        ; reset unstable DOS state calling "Get DOS verify state"
        mov     ah,54h
        int     21h     ; Return (not needed):
                        ;   AL = 0 if flag is Off
                        ;   AL = 1 if flag is On
        mov     bp, sp
        or      byte ptr [bp+16h],1     ; raise Carry flag (indicates DOS error)
        pop     ax
        pop     bx
        pop     cx
        pop     dx
        pop     si
        pop     di
        pop     bp
        pop     ds
        pop     es
        iret            ; return to application

INT24_TEXT ends

end

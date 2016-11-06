; cpu.asm -- CPU-specific functions.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large,pascal
.386

DGROUP group _DATA

_DATA segment word public use16 'DATA'
_DATA ends

CPU_TEXT segment word public use16 'CODE'
assume cs:CPU_TEXT,ds:DGROUP

public isCPU_i386
isCPU_i386 proc far
        ; Now we check if a i386 or higher is present ...
        mov     ax,7000h
        push    ax
        popf
        pushf
        pop     ax
        and     ax,7000h
        jz      failed
        xor     ax,ax
        inc     ax      ; success
failed:
        ret
isCPU_i386 endp

CPU_TEXT ends

end

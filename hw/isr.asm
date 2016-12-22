; isr.asm - interrupt service routines.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large,pascal
.386p
.387

extern _ISRCallback: far

deflabel macro name, index
    &name&index:
endm

defoff macro name, index
    dw offset &name&index
endm

DGROUP group _DATA

ISR_TEXT segment word public use16 'CODE'
assume cs:ISR_TEXT, ds:DGROUP, ss:DGROUP

i=0
repeat 16
    align 2
    deflabel ISR_, %i
    push    eax
    mov     al,i
    jmp     short ISR
    i=i+1
endm

align 2
ISR:
        push    ebx
        push    ecx
        push    edx
        push    ebp
        push    esi
        push    edi
        push    ds
        push    es
        push    fs
        push    gs

        xor     ah,ah
        push    ax
        mov     ax,DGROUP
        mov     ds,ax
        call    far ptr _ISRCallback

        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     edi
        pop     esi
        pop     ebp
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        iret

public getISR
getISR proc far ch: byte
        movzx   bx,[ch]
        shl     bx,1
        mov     ax,word ptr [ISR_tab][bx]
        mov     dx,cs
        ret
getISR endp

ISR_TEXT ends

_DATA segment word public use16 'DATA'
assume cs:_DATA

i=0
ISR_tab:
repeat 16
    defoff ISR_, %i
    i=i+1
endm

_DATA ends

end

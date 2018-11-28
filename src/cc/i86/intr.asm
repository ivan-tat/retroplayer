.387

extrn _cc_DoINTR: byte

DGROUP group _DATA

INTR_TEXT segment byte public use16 'CODE'
assume cs:INTR_TEXT, ds:DGROUP, ss:DGROUP

public cc_intr
cc_intr proc far
; int num (ax)
; union REGPACK *regs (cx:bx)
    push    dx
    push    si
    push    di
    mov     si,ax
    mov     dx,cx
    mov     ax,bx
    mov     bx,si
    call    far ptr _cc_DoINTR
    pop     di
    pop     si
    pop     dx
    ret
cc_intr endp

INTR_TEXT ends

_DATA segment word public use16 'DATA'
_DATA ends

end

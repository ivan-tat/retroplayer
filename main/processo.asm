.model large,pascal
.386

DGROUP group _DATA

_DATA segment word public use16 'DATA'
_DATA ends

PROCESSO_TEXT segment word public use16 'CODE'
assume cs:PROCESSO_TEXT,ds:DGROUP,ss:DGROUP

public Check386
Check386 proc near
        ; Now we check if a 386 or higher is present ...
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
Check386 endp

PROCESSO_TEXT ends

end

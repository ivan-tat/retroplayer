; sndisr_.asm - interrupt service routine for sound card.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large,pascal
.386p
.387

public SoundHWISR
extrn SoundHWISRCallback:dword

DGROUP group _DATA

SNDISR__TEXT segment word public use16 'CODE'
assume cs:SNDISR__TEXT, ds:DGROUP, ss:DGROUP

SoundHWISR:
        push    eax
        push    ecx
        push    edx
        push    ebx
        push    ebp
        push    esi
        push    edi
        push    ds
        push    es
        push    fs
        push    gs

        mov     ax,DGROUP
        mov     ds,ax
        mov     ax,word ptr [SoundHWISRCallback]
        or      ax,word ptr [SoundHWISRCallback+2]
        jz      near ptr SoundHWISR$skip
        call    dword ptr SoundHWISRCallback
SoundHWISR$skip:

        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        pop     edx
        pop     ecx
        pop     eax
        iret

SNDISR__TEXT ends

_DATA segment word public use16 'DATA'
assume cs:_DATA
_DATA ends

end

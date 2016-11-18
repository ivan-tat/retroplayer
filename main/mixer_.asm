; mixer_.asm -- mixer functions.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large,pascal
.386

DGROUP group _DATA

include mixvars.def

_DATA segment word public use16 'DATA'

dwofs macro name, no
        dw      offset &name&no
endm

LOOP_MONO_LENGTH equ 16
LOOP_MONO_SHIFT equ 4

; mono innerloop table:
counter = 0
innerloop_mono_tab label word
rept LOOP_MONO_LENGTH
        dwofs   innerloop_mono, %counter
        counter = counter + 1
endm

LOOP_STEREO_LENGTH equ 16
LOOP_STEREO_SHIFT equ 4

; stereo innerloop table:
counter = 0
innerloop_stereo_tab label word
rept LOOP_STEREO_LENGTH
        dwofs   innerloop_stereo, %counter
        counter = counter + 1
endm

_DATA ends

MIXER__TEXT segment word public use16 'CODE'
assume cs:MIXER__TEXT,ds:DGROUP,ss:DGROUP

public _mixCalcSampleStep
_mixCalcSampleStep proc far _wPeriod: word
local Result: dword
; IN  : DS = _DATA
; OUT : DX:AX = sample step
        push    eax
        push    ecx
        push    edx
        movzx   eax,[_wPeriod]
        movzx   edx,[Userate]
        mul     edx                 ; EAX = Userate*Period
        mov     ecx,eax
        mov     edx,0dah
        mov     eax,77900000h       ; EDX:EAX = 1712*8363*10000h
        div     ecx
        ;        1712 * 8363 * 10000h
        ; EAX = ----------------------
        ;        Userate * Period
        mov     [Result],eax
        pop     edx
        pop     ecx
        pop     eax
        mov     ax,word ptr [Result]
        mov     dx,word ptr [Result+2]
        ret
_mixCalcSampleStep endp

public _MixSampleMono8
_MixSampleMono8:
        ; ES:SI - pointer to tickbuffer
        ; GS:DI - pointer to sampledata
        ; FS:BX - pointer to volumetable
        ; DX    - decision part of current position in sample
        ; DI    - integer part of current position in sample
        ; BH    - volume of instrument
        ; CX    - number of values to calc
        ; BP    - under use, but not in inner loop <- not optimized
        push    ds
        push    fs
        push    bp
        mov     ax,cx
        and     ax,LOOP_MONO_LENGTH-1
        shr     cx,LOOP_MONO_SHIFT
        inc     cx
        neg     ax
        add     ax,LOOP_MONO_LENGTH
        cmp     ax,LOOP_MONO_LENGTH
        jne     no0
        dec     cx
        xor     ax,ax
no0:
        shl     ax,1
        sub     si,ax
        ;sub     si,ax
        xchg    ax,bx
        mov     bx,ds:[innerloop_mono_tab+bx]
        xchg    ax,bx
        ; before jump arround - swap fs,ds
        push    fs
        push    ds
        pop     fs
        pop     ds
        jmp     ax

align 2

innerloop_mono macro no
innerloop_mono&no:
        mov     bl,gs:[di]
        add     edi,edx
        adc     di,0
        mov     ax,ds:[ebx+ebx]  ; convert samplevalue with volumetable
        add     es:[si+pos],ax     ; mix value to other channels
endm

z = 0
pos = 0
rept LOOP_MONO_LENGTH
    innerloop_mono %z
    z = z + 1
    pos = pos + 2
endm
        add     si,LOOP_MONO_LENGTH*2
        dec     cx
        jnz     innerloop_mono0
        pop     bp
        pop     fs
        pop     ds
        retf

public _MixSampleStereo8
_MixSampleStereo8:
        ; ES:SI - pointer to tickbuffer
        ; GS:DI - pointer to sampledata
        ; FS:BX - pointer to volumetable
        ; DX    - decision part of current position in sample
        ; DI    - integer part of current position in sample
        ; BH    - volume of instrument
        ; CX    - number of values to calc
        ; BP    - under use, but not in inner loop <- not optimized
        push    ds
        push    fs
        push    bp
        mov     ax,cx
        and     ax,LOOP_STEREO_LENGTH-1
        shr     cx,LOOP_STEREO_SHIFT
        inc     cx
        neg     ax
        add     ax,LOOP_STEREO_LENGTH
        cmp     ax,LOOP_STEREO_LENGTH
        jne     _no0
        dec     cx
        xor     ax,ax
_no0:
        shl     ax,1
        sub     si,ax
        sub     si,ax
        xchg    ax,bx
        mov     bx,ds:[innerloop_stereo_tab+bx]
        xchg    ax,bx
        ; before jump arround - swap fs,ds
        push    ds
        push    fs
        pop     ds
        pop     fs
        jmp     ax

align 2

innerloop_stereo macro no
innerloop_stereo&no:
        mov     bl,gs:[di]          ; byte out of the sample
        add     edi,edx             ; next position in sample
        adc     di,0                ; <- I need this !!
        mov     ax,ds:[ebx+ebx]     ; convert samplevalue with volumetable
        add     es:[si+pos],ax      ; mix value to other channels
endm

z = 0
pos = 0
rept LOOP_STEREO_LENGTH
    innerloop_stereo %z
    z = z + 1
    pos = pos + 4
endm
        add     si,LOOP_STEREO_LENGTH*4
        dec     cx
        jnz     innerloop_stereo0
        pop     bp
        pop     fs
        pop     ds
        retf

MIXER__TEXT ends

end

; mixer_.asm -- mixer functions.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large,pascal
.386

DGROUP group _DATA

include mixtypes.def
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

public _MixSampleMono8
_MixSampleMono8 proc far _dOutBuf: dword, _wSmpInfo: word, _wVolTabSeg: word, _bVol: byte, _wCount: word
local _count: word
local _step: dword
        push    si
        push    di
        push    ds
        push    fs
        mov     bx,[_wSmpInfo]
        mov     gs,word ptr ss:[bx][TPlaySampleInfo.dData+2]
        mov     edi,ss:[bx][TPlaySampleInfo.dPos]
        rol     edi,16
        add     di,word ptr ss:[bx][TPlaySampleInfo.dData]
        mov     edx,ss:[bx][TPlaySampleInfo.dStep]
        rol     edx,16
        mov     cx,[_wCount]
        mov     ax,cx
        and     ax,LOOP_MONO_LENGTH-1
        shr     cx,LOOP_MONO_SHIFT
        inc     cx
        neg     ax
        add     ax,LOOP_MONO_LENGTH
        cmp     ax,LOOP_MONO_LENGTH
        jne     _MixSampleMono8@_no0
        dec     cx
        xor     ax,ax
_MixSampleMono8@_no0:
        mov     [_count],cx
        shl     ax,1
        les     si,[_dOutBuf]
        sub     si,ax
        ;sub     si,ax
        mov     bx,ax
        mov     ax,ds:[innerloop_mono_tab+bx]
        xor     ebx,ebx
        mov     bh,[_bVol]
        mov     fs,[_wVolTabSeg]
        ; before jump arround - swap fs,ds
        push    fs
        push    ds
        pop     fs
        pop     ds
        jmp     ax

align 2

innerloop_mono macro no
innerloop_mono&no:
        mov     bl,gs:[di]      ; get PCM value
        add     edi,[_step]     ; next sample pos
        adc     di,0            ; next sample pos (carry flag into account)
        mov     ax,ds:[ebx+ebx] ; convert PCM value with volumetable
        add     es:[si+pos],ax  ; mix result to out buffer
endm

z = 0
pos = 0
rept LOOP_MONO_LENGTH
    innerloop_mono %z
    z = z + 1
    pos = pos + 2
endm
        add     si,LOOP_MONO_LENGTH*2
        dec     [_count]
        jnz     innerloop_mono0

        mov     bx,[_wSmpInfo]
        sub     di,word ptr ss:[bx][TPlaySampleInfo.dData]
        rol     edi,16
        mov     ss:[bx][TPlaySampleInfo.dPos],edi

        pop     fs
        pop     ds
        pop     di
        pop     si
        ret
_MixSampleMono8 endp

public _MixSampleStereo8
_MixSampleStereo8 proc far _dOutBuf: dword, _wSmpInfo: word, _wVolTabSeg: word, _bVol: byte, _wCount: word
local _count: word
local _step: dword
        push    si
        push    di
        push    ds
        push    fs
        mov     bx,[_wSmpInfo]
        mov     gs,word ptr ss:[bx][TPlaySampleInfo.dData+2]
        mov     edi,ss:[bx][TPlaySampleInfo.dPos]
        rol     edi,16
        add     di,word ptr ss:[bx][TPlaySampleInfo.dData]
        mov     edx,ss:[bx][TPlaySampleInfo.dStep]
        rol     edx,16
        mov     [_step],edx
        mov     cx,[_wCount]
        mov     ax,cx
        and     ax,LOOP_STEREO_LENGTH-1
        shr     cx,LOOP_STEREO_SHIFT
        inc     cx
        neg     ax
        add     ax,LOOP_STEREO_LENGTH
        cmp     ax,LOOP_STEREO_LENGTH
        jne     _MixSampleStereo8@_no0
        dec     cx
        xor     ax,ax
_MixSampleStereo8@_no0:
        mov     [_count],cx
        shl     ax,1
        les     si,[_dOutBuf]
        sub     si,ax
        sub     si,ax
        mov     bx,ax
        mov     ax,ds:[innerloop_stereo_tab+bx]
        xor     ebx,ebx
        mov     bh,[_bVol]
        mov     fs,[_wVolTabSeg]
        ; before jump arround - swap fs,ds
        push    ds
        push    fs
        pop     ds
        pop     fs
        jmp     ax

align 2

innerloop_stereo macro no
innerloop_stereo&no:
        mov     bl,gs:[di]      ; get PCM value
        add     edi,[_step]     ; next sample pos
        adc     di,0            ; next sample pos (carry flag into account)
        mov     ax,ds:[ebx+ebx] ; convert PCM value with volumetable
        add     es:[si+pos],ax  ; mix result to out buffer
endm

z = 0
pos = 0
rept LOOP_STEREO_LENGTH
    innerloop_stereo %z
    z = z + 1
    pos = pos + 4
endm
        add     si,LOOP_STEREO_LENGTH*4
        dec     [_count]
        jnz     innerloop_stereo0

        mov     bx,[_wSmpInfo]
        sub     di,word ptr ss:[bx][TPlaySampleInfo.dData]
        rol     edi,16
        mov     ss:[bx][TPlaySampleInfo.dPos],edi

        pop     fs
        pop     ds
        pop     di
        pop     si
        ret
_MixSampleStereo8 endp

MIXER__TEXT ends

end

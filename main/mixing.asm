; mixing.asm -- mixer functions.
;
; This is free and unencumbered software released into the public domain.
; For more information, please refer to <http://unlicense.org>.

.model large,pascal
.386

DGROUP group _DATA

include ..\dos\emstool.def
include ..\blaster\sbctl.def
include s3mtypes.def
include mixtypes.def
include s3mvars.def
include effvars.def
include mixvars.def
include fillvars.def
include voltab.def
include effects.def
include mixer.def
include mixer_.def
include readnote.def

_DATA segment word public use16 'DATA'
_DATA ends

MIXING_TEXT segment word public use16 'CODE'
assume cs:MIXING_TEXT,ds:DGROUP

include border.inc

mapData proc far _wSeg: word, _wLen: word
local _wLogPage: word
local _wPhysPage: word
        push    si
        push    es  ; ES is lost after INT 67h
        ; well now check if in EMS :
        mov     ax,[_wSeg]
        cmp     ax,0f000h
        jb      mapData@_exit
        and     ax,00fffh
        mov     [_wLogPage],ax
        mov     [_wPhysPage],0
mapData@_nextPage:
        push    [smpEMShandle]  ; handle
        push    [_wLogPage]     ; logical page
        push    [_wPhysPage]    ; physical page
        call    EmsMap
        test    al,al
        jnz     mapData@_ok
        ; return NULL pointer because EMS driver does not work correct
        xor     ax,ax
        jmp     mapData@_exit
mapData@_ok:
        inc     [_wLogPage]
        inc     [_wPhysPage]
        sub     [_wLen],16*1024
        jnc     mapData@_nextPage
        mov     ax,[frameseg]
mapData@_exit:
        mov     dx,ax
        xor     ax,ax
        pop     es
        pop     si
        ret
mapData endp

getBufPosFromCount proc near
; IN:  ax = number of samples
;      ds = _DATA
; OUT: ax = buffer offset (in bytes)
        cmp     [_16bit],0
        je      getBufPosFromCount@_no16bit
        shl     ax,1
getBufPosFromCount@_no16bit:
        cmp     [stereo],0
        je      getBufPosFromCount@_noStereo
        shl     ax,1
getBufPosFromCount@_noStereo:
        ret
getBufPosFromCount endp

getCountFromBufPos proc near
; IN:  ax = buffer offset (in bytes)
;      ds = _DATA
; OUT: ax = number of samples
        cmp     [_16bit],0
        je      getCountFromBufPos@_no16bit
        shr     ax,1
getCountFromBufPos@_no16bit:
        cmp     [stereo],0
        je      getCountFromBufPos@_noStereo
        shr     ax,1
getCountFromBufPos@_noStereo:
        ret
getCountFromBufPos endp

public calc_tick
calc_tick proc far
local nextPosition: word
local sample2calc:  word    ; N of samples per channel (mono/left/right) to calculate
local curchannel:   byte
local calleffects:  byte
local _outBufOff: word
local _smpInfo: TPlaySampleInfo

        ; first fill tickbuffer with zero
        xor     di,di
        mov     ax,word ptr [tickbuffer+2]
        mov     es,ax
        xor     ax,ax
        mov     cx,[DMArealBufsize+2]
        setborder     3
        rep stosw
        setborder     1
        mov     [nextPosition],0
        mov     [calleffects],0
        cmp     [mixTickSamplesPerChannelLeft],0
        jnz     _continuecalc

calc_tick_anewtick:
        mov     ax,[mixTickSamplesPerChannel]
        mov     [mixTickSamplesPerChannelLeft],ax
        mov     [calleffects],1
        cmp     [curtick],1
        ja      calc_tick_decrtick

        cmp     [patterndelay],0
        je      calc_tick_nodelay     ; or pattern delay done ...

        dec     [patterndelay]
        jz      calc_tick_nodelay

        dec     [curline]

calc_tick_nodelay:
        setborder 4
        call    readnewnotes
        setborder 1
        jmp     _continuecalc

calc_tick_decrtick:
        dec     [curtick]

_continuecalc:
        cmp     [EndOfSong],1
        je      _afterall

        mov     al,[usedchannels]
        mov     [curchannel],al

        mov     ax,[mixTickSamplesPerChannelLeft]
        call    getBufPosFromCount
        mov     cx,[DMArealBufsize+2]
        sub     cx,[nextPosition]
        cmp     cx,ax
        jbe     _cantfinishtick
        mov     cx,ax   ; finish that Tick and loop to fill the whole tickbuffer
_cantfinishtick:
        mov     ax,cx
        call    getCountFromBufPos
        mov     [sample2calc],ax

        test    ax,ax
        jz      _afterall

        lea     si,[Channel]

_chnLoop:
        cmp     byte ptr [SI][TChannel.bChannelType],0
        je      _nextchannel

        cmp     byte ptr [SI][TChannel.bChannelType],2
        ja      _nextchannel

        cmp     [calleffects],0
        je      _noeff

        ; do effects for this channel :
        mov     al,[curtick]
        cmp     al,[curspeed]
        je      _noeff_forfirst

_doeff:
        push    ds  ; FP_SEG(channel_t *)
        push    si  ; FP_OFF(channel_t *)
        call    chn_effTick
_noeff:
_noeff_forfirst:
        ; check if mixing :
        cmp     byte ptr [SI][TChannel.bEnabled],0
        je      _nextchannel

        push    [SI][TChannel.wSmpSeg]
        push    [SI][TChannel.wSmpLoopEnd]
        call    mapData
        mov     bx,ax
        or      bx,dx
        jz      _nextchannel    ; skip channel if EMS driver does not work correct
        mov     word ptr [_smpInfo.dData],ax
        mov     word ptr [_smpInfo.dData+2],dx

        mov     ax,[nextPosition]
        shl     ax,1
        cmp     [stereo],0
        je      _skip_2
        ; oh well - now stereo position
        cmp     byte ptr [SI][TChannel.bChannelType],1
        je      _leftside
        add     ax,2
_leftside:
_skip_2:
        mov     [_outBufOff],ax

        mov     edi,[SI][TChannel.dSmpPos]
        mov     [_smpInfo.dPos],edi
        rol     edi,16

        mov     edx,[SI][TChannel.dSmpStep]
        mov     [_smpInfo.dStep],edx
        rol     edx,16

        ; first check for correct position inside sample
        cmp     di,[SI][TChannel.wSmpLoopEnd]
        jae     _sampleends

        mov     ax,word ptr [tickbuffer+2]
        push    ax  ; FP_SEG(outbuf)
        mov     ax,word ptr [tickbuffer]
        add     ax,[_outBufOff]
        push    ax  ; FP_OFF(outbuf)
        push    ss  ; FP_SEG(smpInfo)
        lea     ax,[_smpInfo]
        push    ax  ; FP_OFF(smpInfo)
        mov     ax,word ptr [volumetableptr+2]
        push    ax  ; FP_SEG(volTab)
        movzx   ax,[SI][TChannel.bSmpVol]
        push    ax  ; uint16 vol
        mov     ax,[sample2calc]
        push    ax  ; uint16 count

        cmp     [stereo],0
        je      _skip_3

        call    _MixSampleStereo8
        jmp     _skip_4

_skip_3:
        call    _MixSampleMono8

_skip_4:
        mov     edi,[_smpInfo.dPos]
        rol     edi,16
        cmp     di,[SI][TChannel.wSmpLoopEnd]
        jae     _sampleends

_back2main:
        rol     edi,16
        mov     [SI][TChannel.dSmpPos],edi

_nextchannel:
        add     si,size TChannel
        dec     [curchannel]
        jnz     _chnLoop

        mov     ax,[sample2calc]
        sub     [mixTickSamplesPerChannelLeft],ax

        call    getBufPosFromCount
        add     [nextPosition],ax

        mov     ax,[DMArealBufsize+2]
        cmp     [nextPosition],ax
        jb      calc_tick_anewtick

_afterall:
        ret

_sampleends:
        test    byte ptr [SI][TChannel.bSmpFlags],SMPFLAG_LOOP
        jz      _no_loopflag

_tryagain:
        sub     di,[SI][TChannel.wSmpLoopEnd]
        add     di,[SI][TChannel.wSmpLoopStart]
        cmp     di,[SI][TChannel.wSmpLoopEnd]
        jae     _tryagain

        jmp     _back2main

_no_loopflag:
        mov     byte ptr [SI][TChannel.bEnabled],0
        jmp     _back2main

calc_tick endp

MIXING_TEXT ends

end

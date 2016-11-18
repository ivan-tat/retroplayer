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
include s3mvars.def
include effvars.def
include mixvars.def
include fillvars.def
include voltab.def
include mixer_.def
include readnote.def

_DATA segment word public use16 'DATA'

EffectsTable label word
        dw offset eff_none
        dw offset eff_none
        dw offset eff_none
        dw offset eff_none
        dw offset eff_D_VolumeSlide
        dw offset eff_E_PitchDown
        dw offset eff_F_PitchUp
        dw offset eff_G_Portamento
        dw offset eff_H_Vibrato
        dw offset eff_I_Tremor
        dw offset eff_J_Arpeggio
        dw offset eff_K_VibratoVolSlide
        dw offset eff_L_PortamentoVolSlide
        dw offset eff_none
        dw offset eff_none
        dw offset eff_none
        dw offset eff_none
        dw offset eff_Q_Retrigger
        dw offset eff_R_Tremolo
        dw offset eff_S_Special
        dw offset eff_none
        dw offset eff_U_FineVibrato
        dw offset eff_none
        ; W-Z,0-9 - no effects

eff_D_VolumeSlide_tab label word
        dw offset eff_D_VolumeSlide_Down
        dw offset eff_D_VolumeSlide_Up
        dw offset eff_none
        dw offset eff_none

eff_E_PitchDown_tab label word
        dw offset eff_E_PitchDown_Down
        dw offset eff_none
        dw offset eff_none

eff_F_PitchUp_tab label word
        dw offset eff_F_PitchUp_Up
        dw offset eff_none
        dw offset eff_none

eff_Q_Retrigger_tab label word
        dw offset eff_none
        dw offset eff_Q_Retrigger_SlideDown
        dw offset eff_Q_Retrigger_Use2div3
        dw offset eff_Q_Retrigger_Use1div2
        dw offset eff_Q_Retrigger_SlideUp
        dw offset eff_Q_Retrigger_Use3div2
        dw offset eff_Q_Retrigger_Use2div1

eff_S_Special_tab label word
        dw offset eff_none      ; N/A
        dw offset eff_none      ; set filter
        dw offset eff_none      ; set glissando
        dw offset eff_none      ; set finetune
        dw offset eff_none      ; set vibrato waveform
        dw offset eff_none      ; set tremolo waveform
        dw offset eff_none      ; N/A
        dw offset eff_none      ; N/A
        dw offset eff_none      ; panning
        dw offset eff_none      ; N/A
        dw offset eff_none      ; stereo control
        dw offset eff_none      ; Pattern loop things
        dw offset eff_S_Special_NoteCut
        dw offset eff_S_Special_NoteDelay
        dw offset eff_none      ; Pattern delay
        dw offset eff_none      ; funkrepeat

_DATA ends

MIXING_TEXT segment word public use16 'CODE'
assume cs:MIXING_TEXT,ds:DGROUP

include border.inc

public calc_tick
calc_tick proc far
local nextPosition: word
local sample2calc:  word    ; in mono number of bytes/ in stereo number of words
local curchannel:   byte
local calleffects:  byte

        ; first fill tickbuffer with ZERO = 2048
        ; for 16bit play then ofcourse a bit different value ...
        ; just only for 8bit play mode
        mov     ax,word ptr [offset tickbuffer+2]
        mov     es,ax
        mov     ax,2048
        xor     di,di
        mov     cx,[DMArealBufsize+2]
        setborder     3
        rep stosw
        setborder     1
        mov     [nextPosition],0
        mov     [calleffects],0
        cmp     [TickBytesLeft],0
        jnz     _continuecalc

calc_tick_anewtick:
        mov     ax,[BPT]
        mov     [TickBytesLeft],ax
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
        mov     ax,word ptr [offset tickbuffer+2]
        mov     es,ax
        cmp     [EndOfSong],1
        je      _afterall

        mov     al,[usedchannels]
        mov     [curchannel],al
        ; number of bytes (?) we calc for every tick:
        mov     ax,[TickBytesLeft]
        cmp     [stereo],0
        je      _skip_0

        shl     ax,1    ; only in stereo

_skip_0:
        mov     cx,[DMArealBufsize+2]
        sub     cx,[nextPosition]
        cmp     cx,ax
        jbe     _cantfinishtick

        mov     cx,ax     ; finish that Tick and loop to fill the whole tickbuffer

_cantfinishtick:
        mov     [sample2calc],cx
        cmp     cx,0
        je      _afterall

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
        mov     bx,[SI][TChannel.wCommand]
        cmp     bx,255*2
        je      _noeff

        call    [EffectsTable+bx]

_noeff:
_noeff_forfirst:
        ; check if mixing :
        cmp     byte ptr [SI][TChannel.bEnabled],0
        je      _nextchannel

        ; well now check if in EMS :
        mov     ax,[SI][TChannel.wSmpSeg]
        cmp     ax,0f000h
        jb      _noEMSsample

        and     ax,0fffh
        xor     bx,bx
        mov     cx,[SI][TChannel.wSmpLoopEnd]

_onemorepage:
        push    ax
        push    bx
        push    cx
        push    si
        push    bp
        push    es  ; ES is lost after INT 67h
        push    word ptr [smpEMShandle] ; handle
        push    ax  ; logical page
        push    bx  ; physical page
        call    EmsMap
        test    al,al
        jnz     _noemsprob
        ; cause an exception "division by zero" because EMS driver does not work correct
        xor     dl,dl
        div     dl

_noemsprob:
        pop     es
        pop     bp
        pop     si
        pop     cx
        pop     bx
        pop     ax
        inc     ax
        inc     bl
        sub     cx,16*1024
        jnc     _onemorepage

        mov     ax,[frameseg]

_noEMSsample:
        mov     gs,ax

        lfs     ax,[volumetableptr]

        xor     ebx,ebx
        mov     bh,[SI][TChannel.bSmpVol]

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
        mov     cx,[sample2calc]
        cmp     [stereo],0
        je      _skip_5

        shr     cx,1    ; only in stereo

_skip_5:
        mov     edi,[SI][TChannel.dSmpPos]
        rol     edi,16

        mov     edx,[SI][TChannel.dSmpStep]
        rol     edx,16

        ; first check for correct position inside sample
        cmp     di,[SI][TChannel.wSmpLoopEnd]
        jae     _sampleends

        cmp     [stereo],0
        je      _skip_3

        push    si
        mov     si,ax
        call    _MixSampleStereo8
        pop     si
        jmp     _skip_4

_skip_3:
        push    si
        mov     si,ax
        call    _MixSampleMono8
        pop     si

_skip_4:
_aftercalc:
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
        add     [nextPosition],ax
        cmp     [stereo],0
        je      _skip_1

        shr     ax,1    ; only in stereo

_skip_1:
        sub     [TickBytesLeft],ax
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

calcnewSF proc near
; In: ax = period
;     si = channel offset
;     ds = _DATA
        ; now calc new frequency step for this period
        mov     [SI][TChannel.wSmpPeriod],ax
        cmp     ax,0
        je      donotcalc
        call    _mixCalcSampleStep
        mov     [SI][TChannel.dSmpStep],eax
donotcalc:
        ret
calcnewSF endp

voltest proc near
; In: al = volume
        cmp      al,64
        jb       voltest_done
        mov      al,63
voltest_done:
        mov      [SI][TChannel.bSmpVol],al
        ret
voltest endp

eff_none proc near
        ret
eff_none endp

eff_D_VolumeSlide proc near
        mov     bx,[SI][TChannel.wCommand2]
        call    [eff_D_VolumeSlide_tab+bx]
        ret
eff_D_VolumeSlide endp

eff_D_VolumeSlide_Down proc near
        mov     al,[SI][TChannel.bParameter]
        and     al,0fh
        sub     [SI][TChannel.bSmpVol],al
        jnc     eff_D_VolumeSlide_Down_exit
        mov     byte ptr [SI][TChannel.bSmpVol],0
eff_D_VolumeSlide_Down_exit:
        ret
eff_D_VolumeSlide_Down endp

eff_D_VolumeSlide_Up proc near
        mov     al,[SI][TChannel.bParameter]
        shr     al,4
        add     [SI][TChannel.bSmpVol],al
        cmp     byte ptr [SI][TChannel.bSmpVol],64
        jb      eff_D_VolumeSlide_Up_done
        mov     byte ptr [SI][TChannel.bSmpVol],63
eff_D_VolumeSlide_Up_done:
        mov     al,[SI][TChannel.bSmpVol]
        mul     [gvolume]
        shr     ax,6
        mov     [SI][TChannel.bSmpVol],al
        ret
eff_D_VolumeSlide_Up endp

eff_E_PitchDown proc near
        mov     bx,[SI][TChannel.wCommand2]
        call    [eff_E_PitchDown_tab+bx]
        ret
eff_E_PitchDown endp

eff_E_PitchDown_Down proc near
        ; we pitch down, but increase period ! (so check wSmpPeriodHigh)
        mov     ax,[SI][TChannel.wSmpPeriod]
        mov     bl,[SI][TChannel.bParameter]
        xor     bh,bh
        shl     bx,2
        add     ax,bx
        cmp     ax,[SI][TChannel.wSmpPeriodHigh]
        jb      eff_E_PitchDown_Down_1
        mov     ax,[SI][TChannel.wSmpPeriodHigh]
eff_E_PitchDown_Down_1:
        call    calcnewSF
        ret
eff_E_PitchDown_Down endp

eff_F_PitchUp proc near
        mov     bx,[SI][TChannel.wCommand2]
        call    [eff_F_PitchUp_tab+bx]
        ret
eff_F_PitchUp endp

eff_F_PitchUp_Up proc near
        ; we pitch up, but decrease period ! (so check wSmpPeriodLow)
        mov     ax,[SI][TChannel.wSmpPeriod]
        mov     bl,[SI][TChannel.bParameter]
        xor     bh,bh
        shl     bx,2
        sub     ax,bx
        cmp     ax,[SI][TChannel.wSmpPeriodLow]
        ja      eff_F_PitchUp_Up_1
        mov     ax,[SI][TChannel.wSmpPeriodLow]
eff_F_PitchUp_Up_1:
        call    calcnewSF
        ret
eff_F_PitchUp_Up endp

eff_G_Portamento proc near
        mov       bl,[SI][TChannel.bPortParam]
        xor       bh,bh
        shl       bx,2    ; <- use amiga slide = para*4
        mov       ax,[SI][TChannel.wSmpPeriod]
        cmp       ax,[SI][TChannel.wSmpPeriodDest]
        jg        porta_down
        add       ax,bx
        cmp       ax,[SI][TChannel.wSmpPeriodDest]
        jle       eff_G_Portamento_done
        mov       ax,[SI][TChannel.wSmpPeriodDest]
        jmp       eff_G_Portamento_done
porta_down:
        sub       ax,bx
        cmp       ax,[SI][TChannel.wSmpPeriodDest]
        jge       eff_G_Portamento_done
        mov       ax,[SI][TChannel.wSmpPeriodDest]
eff_G_Portamento_done:
        call      calcnewSF
        ret
eff_G_Portamento endp

eff_H_Vibrato proc near
        cmp       byte ptr [SI][TChannel.bEnabled],0
        je        eff_H_Vibrato_exit
        ; next position in table:
        mov       al,[SI][TChannel.bVibParam]
        mov       dl,al
        and       dl,0fh
        shr       al,4
        mov       bl,[SI][TChannel.bTabPos]
        add       bl,al
        cmp       bl,64
        jb        endoftest
        sub       bl,64
endoftest:
        mov       [SI][TChannel.bTabPos],bl
        xor       bh,bh
        add       bx,[SI][TChannel.wVibTab]
        mov       al,ds:[bx]
        imul      dl
        sar       ax,4
        mov       bx,[SI][TChannel.wSmpPeriodOld]
        add       ax,bx
        call      calcnewSF
eff_H_Vibrato_exit:
        ret
eff_H_Vibrato endp

eff_I_Tremor proc near
        ret
eff_I_Tremor endp

eff_J_Arpeggio proc near
        mov     bl,[SI][TChannel.bArpPos]
        xor     bh,bh
        inc     bx
        cmp     bx,3
        jb      inside
        xor     bx,bx
inside:
        mov     [SI][TChannel.bArpPos],bl
        shl     bx,2
        add     si,bx
        mov     eax,[SI][TChannel.dArpSmpSteps]
        sub     si,bx
        mov     [SI][TChannel.dSmpStep],eax
        ret
eff_J_Arpeggio endp

eff_K_VibratoVolSlide proc near
        call    eff_D_VolumeSlide
        call    eff_H_Vibrato
        ret
eff_K_VibratoVolSlide endp

eff_L_PortamentoVolSlide proc near
        call    eff_D_VolumeSlide
        call    eff_G_Portamento
        ret
eff_L_PortamentoVolSlide endp

eff_Q_Retrigger proc near
        cmp     byte ptr [SI][TChannel.bRetrigTicks],0
        jz      doretrigg
        dec     byte ptr [SI][TChannel.bRetrigTicks]
        jnz     eff_Q_Retrigger_exit
doretrigg:
        xor     eax,eax
        mov     [SI][TChannel.dSmpPos],eax
        mov     al,[SI][TChannel.bParameter]
        and     al,0fh
        jz      eff_Q_Retrigger_exit
        mov     [SI][TChannel.bRetrigTicks],al
        mov     bx,[SI][TChannel.wCommand2]
        call    [eff_Q_Retrigger_tab+bx]
eff_Q_Retrigger_exit:
        ret
eff_Q_Retrigger endp

eff_Q_Retrigger_SlideDown proc near
        mov     cl,[SI][TChannel.bParameter]
        shr     cl,4
        mov     al,1
        shl     al,cl
        sub     [SI][TChannel.bSmpVol],al
        jnc     eff_Q_Retrigger_SlideDown_exit
        mov     byte ptr [SI][TChannel.bSmpVol],0
eff_Q_Retrigger_SlideDown_exit:
        ret
eff_Q_Retrigger_SlideDown endp

eff_Q_Retrigger_Use2div3 proc near
; (it's 5/8 in real life ;)
        mov      al,[SI][TChannel.bSmpVol]
        mov      ah,al
        shl      al,2             ; al = 4*volume , ah = volume
        add      al,ah            ; al = 5*volume
        shr      al,3             ; al = 5*volume/8
        mov      [SI][TChannel.bSmpVol],al
        ret
eff_Q_Retrigger_Use2div3 endp

eff_Q_Retrigger_Use1div2 proc near
        shr      byte ptr [SI][TChannel.bSmpVol],1
        ret
eff_Q_Retrigger_Use1div2 endp

eff_Q_Retrigger_SlideUp proc near
        mov     cl,[SI][TChannel.bParameter]
        shr     cl,4
        mov     al,1
        shl     al,cl
        add     al,[SI][TChannel.bSmpVol]
        call    voltest
        ret
eff_Q_Retrigger_SlideUp endp

eff_Q_Retrigger_Use3div2 proc near
        mov     al,[SI][TChannel.bSmpVol]
        mov     ah,al
        add     al,al            ; al = 2*volume , ah = volume
        add     al,ah            ; al = 3*volume
        shr     al,1             ; al = 3*volume/2
        call    voltest
        ret
eff_Q_Retrigger_Use3div2 endp

eff_Q_Retrigger_Use2div1 proc near
        mov     al,[SI][TChannel.bSmpVol]
        shl     al,1
        call    voltest
        ret
eff_Q_Retrigger_Use2div1 endp

eff_R_Tremolo proc near
        ; next position in table:
        mov       al,[SI][TChannel.bParameter]
        mov       dl,al
        and       dl,0fh
        shr       al,4
        mov       bl,[SI][TChannel.bTabPos]
        add       bl,al
        cmp       bl,64
        jb        endoftest2
        sub       bl,64
endoftest2:
        mov       [SI][TChannel.bTabPos],bl
        xor       bh,bh
        add       bx,[SI][TChannel.wTrmTab]
        mov       al,ds:[bx]
        imul      dl
        sar       ax,6
        mov       bl,[SI][TChannel.bSmpVolOld]
        xor       bh,bh
        add       bx,ax
        cmp       bx,63
        jng       ok1
        mov       bl,63
ok1:
        cmp       bx,0
        jnl       ok2
        mov       bl,0
ok2:
        mov       [SI][TChannel.bSmpVol],bl
        ret
eff_R_Tremolo endp

eff_S_Special proc near
        mov     bx,[SI][TChannel.wCommand2]
        call    [eff_S_Special_tab+bx]
        ret
eff_S_Special endp

eff_S_Special_NoteCut proc near
        dec     byte ptr [SI][TChannel.bDelayTicks]
        jnz     eff_S_Special_NoteCut_exit
        mov     byte ptr [SI][TChannel.bEnabled],0       ;disable it ...
eff_S_Special_NoteCut_exit:
        ret
eff_S_Special_NoteCut endp

eff_S_Special_NoteDelay proc near
        push    fs  ; segment to volumetable, but we may destroy it here ...
        dec     byte ptr [SI][TChannel.bDelayTicks]
        jnz     eff_S_Special_NoteDelay_exit
        ; Ok now we have to calc things for the new note/instr ...
        ; 1. Setup Instrument
        mov     al,[SI][TChannel.bSavIns]
        cmp     al,00
        je      nonewinst
        mov     [SI][TChannel.bIns],al
        push    ds
        push    si
        xor     ah,ah
        push    ax
        call    SetupNewInst
nonewinst:
        mov     al,[SI][TChannel.bSavNote]
        cmp     al,0ffh
        je      no_newnote
        cmp     al,0feh
        jne     normal_note
        mov     byte ptr [SI][TChannel.bEnabled],0     ; stop mixing
        jmp     no_newnote
normal_note:
        mov     byte ptr [SI][TChannel.bEnabled],1     ; yo do mixing
        mov     [SI][TChannel.bNote],al
        call    SetNewNote
no_newnote:
        mov     al,[SI][TChannel.bSavVol]
        cmp     al,0ffh
        je      no_vol
        mul     [gvolume]
        shr     ax,6
        mov     [SI][TChannel.bSmpVol],al
no_vol:
        mov     word ptr [SI][TChannel.wCommand],0    ; <- no more Notedelay
eff_S_Special_NoteDelay_exit:
        pop     fs
        ret
eff_S_Special_NoteDelay endp


eff_U_FineVibrato proc near
        cmp     byte ptr [SI][TChannel.bEnabled],0
        je      eff_U_FineVibrato_exit
        ; next position in table:
        mov     al,[SI][TChannel.bVibParam]
        mov     dl,al
        and     dl,0fh
        shr     al,4
        mov     bl,[SI][TChannel.bTabPos]
        add     bl,al
        cmp     bl,64
        jb      f_endoftest
        sub     bl,64
f_endoftest:
        mov     [SI][TChannel.bTabPos],bl
        xor     bh,bh
        add     bx,[SI][TChannel.wVibTab]
        mov     al,ds:[bx]
        imul    dl
        sar     ax,8
        mov     bx,[SI][TChannel.wSmpPeriodOld]
        add     ax,bx
        call    calcnewSF
eff_U_FineVibrato_exit:
        ret
eff_U_FineVibrato endp

MIXING_TEXT ends

end

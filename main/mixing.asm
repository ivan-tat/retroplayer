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

nextPosition dw ?
sample2calc  dw ?     ; in mono number of bytes/ in stereo number of words
curchannel   db ?
calleffects  db ?

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

CHANOFF equ ds:[bp][channel]

include border.inc

public calc_tick
calc_tick proc far
        push    bp
        ; first fill tickbuffer with ZERO = 2048+offset post8bit
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

        xor     bp,bp

_chnLoop:
        cmp     byte ptr CHANOFF[TChannel.bChannelType],0
        je      _nextchannel

        cmp     byte ptr CHANOFF[TChannel.bChannelType],2
        ja      _nextchannel

        cmp     [calleffects],0
        je      _noeff

        ; do effects for this channel :
        mov     al,[curtick]
        cmp     al,[curspeed]
        je      _noeff_forfirst

_doeff:
        mov     bx,CHANOFF[TChannel.command]
        cmp     bx,255*2
        je      _noeff

        call    [EffectsTable+bx]

_noeff:
_noeff_forfirst:
        ; check if mixing :
        cmp     byte ptr CHANOFF[TChannel.bEnabled],0
        je      _nextchannel

        ; well now check if in EMS :
        mov     ax,CHANOFF[TChannel.SampleSEG]
        cmp     ax,0f000h
        jb      _noEMSsample

        and     ax,0fffh
        xor     bx,bx
        mov     cx,CHANOFF[TChannel.sLoopend]

_onemorepage:
        push    ax
        push    bx
        push    cx
        push    bp
comment #
; somehow this does not work properly...
        push    word ptr [smpEMShandle] ; handle
        push    ax  ; logical page
        push    bx  ; physical page
        call    EmsMap
        test    al,al
        jnz     _noemsprob
#
;comment #
        xchg    bx,ax
        mov     ah,044h
        mov     dx,[smpEMShandle]
        int     67h
        test    ah,ah
        jz      _noemsprob
;#
        ; cause an exception "division by zero" because EMS driver does not work correct
        xor     dl,dl
        div     dl

_noemsprob:
        pop     bp
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

        lfs     si,[volumetableptr]

        xor     ebx,ebx
        mov     bh,CHANOFF[TChannel.bSampleVol]

        mov     si,[nextPosition]
        shl     si,1
        cmp     [stereo],0
        je      _skip_2

        ; oh well - now stereo position
        cmp     byte ptr CHANOFF[TChannel.bChannelType],1
        je      _leftside

        add     si,2

_leftside:
_skip_2:
        mov     cx,[sample2calc]
        cmp     [stereo],0
        je      _skip_5

        shr     cx,1    ; only in stereo

_skip_5:
        mov     edi,CHANOFF[TChannel.sCurpos]
        rol     edi,16

        mov     edx,CHANOFF[TChannel.sStep]
        rol     edx,16

        ; first check for correct position inside sample
        cmp     di,CHANOFF[TChannel.sLoopend]
        jae     _sampleends

        cmp     [stereo],0
        je      _skip_3

        call    _MixSampleStereo8
        jmp     _skip_4

_skip_3:
        call    _MixSampleMono8

_skip_4:
_aftercalc:
        cmp     di,CHANOFF[TChannel.sLoopend]
        jae     _sampleends

_back2main:
        rol     edi,16
        mov     CHANOFF[TChannel.sCurpos],edi

_nextchannel:
        add     bp,size TChannel
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
        pop     bp
        ret

_sampleends:
        cmp     byte ptr CHANOFF[TChannel.sloopflag],0
        je      _no_loopflag

_tryagain:
        sub     di,CHANOFF[TChannel.sloopEnd]
        add     di,CHANOFF[TChannel.sloopstart]
        cmp     di,CHANOFF[TChannel.sloopEnd]
        jae     _tryagain

        jmp     _back2main

_no_loopflag:
        mov     byte ptr CHANOFF[TChannel.bEnabled],0
        jmp     _back2main

calc_tick endp

calcnewSF proc near
; In: ax = period
;     bp = channel offset
;     ds = _DATA
        ; now calc new frequency step for this period
        mov     CHANOFF[TChannel.sPeriod],ax
        cmp     ax,0
        je      donotcalc
        call    _mixCalcSampleStep
        mov     CHANOFF[TChannel.sStep],eax
donotcalc:
        ret
calcnewSF endp

voltest proc near
; In: al = volume
        cmp      al,64
        jb       voltest_done
        mov      al,63
voltest_done:
        mov      CHANOFF[TChannel.bSampleVol],al
        ret
voltest endp

eff_none proc near
        ret
eff_none endp

eff_D_VolumeSlide proc near
        mov     bx,CHANOFF[TChannel.cmd2nd]
        call    [eff_D_VolumeSlide_tab+bx]
        ret
eff_D_VolumeSlide endp

eff_D_VolumeSlide_Down proc near
        mov     al,CHANOFF[TChannel.Parameter]
        and     al,0fh
        sub     CHANOFF[TChannel.bSampleVol],al
        jnc     eff_D_VolumeSlide_Down_exit
        mov     byte ptr CHANOFF[TChannel.bSampleVol],0
eff_D_VolumeSlide_Down_exit:
        ret
eff_D_VolumeSlide_Down endp

eff_D_VolumeSlide_Up proc near
        mov     al,CHANOFF[TChannel.Parameter]
        shr     al,4
        add     CHANOFF[TChannel.bSampleVol],al
        cmp     byte ptr CHANOFF[TChannel.bSampleVol],64
        jb      eff_D_VolumeSlide_Up_done
        mov     byte ptr CHANOFF[TChannel.bSampleVol],63
eff_D_VolumeSlide_Up_done:
        mov     al,CHANOFF[TChannel.bSampleVol]
        mul     [gvolume]
        shr     ax,6
        mov     CHANOFF[TChannel.bSampleVol],al
        ret
eff_D_VolumeSlide_Up endp

eff_E_PitchDown proc near
        mov     bx,CHANOFF[TChannel.cmd2nd]
        call    [eff_E_PitchDown_tab+bx]
        ret
eff_E_PitchDown endp

eff_E_PitchDown_Down proc near
        ; we pitch down, but increase period ! (so check upper_border)
        mov     ax,CHANOFF[TChannel.sPeriod]
        mov     bl,CHANOFF[TChannel.Parameter]
        xor     bh,bh
        shl     bx,2
        add     ax,bx
        cmp     ax,CHANOFF[TChannel.upper_border]
        jb      eff_E_PitchDown_Down_1
        mov     ax,CHANOFF[TChannel.upper_border]
eff_E_PitchDown_Down_1:
        call    calcnewSF
        ret
eff_E_PitchDown_Down endp

eff_F_PitchUp proc near
        mov     bx,CHANOFF[TChannel.cmd2nd]
        call    [eff_F_PitchUp_tab+bx]
        ret
eff_F_PitchUp endp

eff_F_PitchUp_Up proc near
        ; we pitch up, but decrease period ! (so check lower_border)
        mov     ax,CHANOFF[TChannel.sPeriod]
        mov     bl,CHANOFF[TChannel.Parameter]
        xor     bh,bh
        shl     bx,2
        sub     ax,bx
        cmp     ax,CHANOFF[TChannel.lower_border]
        ja      eff_F_PitchUp_Up_1
        mov     ax,CHANOFF[TChannel.lower_border]
eff_F_PitchUp_Up_1:
        call    calcnewSF
        ret
eff_F_PitchUp_Up endp

eff_G_Portamento proc near
        mov       bl,CHANOFF[TChannel.PortPara]
        xor       bh,bh
        shl       bx,2    ; <- use amiga slide = para*4
        mov       ax,CHANOFF[TChannel.sPeriod]
        cmp       ax,CHANOFF[TChannel.wantedPeri]
        jg        porta_down
        add       ax,bx
        cmp       ax,CHANOFF[TChannel.wantedPeri]
        jle       eff_G_Portamento_done
        mov       ax,CHANOFF[TChannel.wantedPeri]
        jmp       eff_G_Portamento_done
porta_down:
        sub       ax,bx
        cmp       ax,CHANOFF[TChannel.wantedPeri]
        jge       eff_G_Portamento_done
        mov       ax,CHANOFF[TChannel.wantedPeri]
eff_G_Portamento_done:
        call      calcnewSF
        ret
eff_G_Portamento endp

eff_H_Vibrato proc near
        cmp       byte ptr CHANOFF[TChannel.bEnabled],0
        je        eff_H_Vibrato_exit
        ; next position in table:
        mov       al,CHANOFF[TChannel.VibPara]
        mov       dl,al
        and       dl,0fh
        shr       al,4
        mov       bl,CHANOFF[TChannel.Tablepos]
        add       bl,al
        cmp       bl,64
        jb        endoftest
        sub       bl,64
endoftest:
        mov       CHANOFF[TChannel.Tablepos],bl
        xor       bh,bh
        add       bx,CHANOFF[TChannel.VibTabOfs]
        mov       al,ds:[bx]
        imul      dl
        sar       ax,4
        mov       bx,CHANOFF[TChannel.Oldperiod]
        add       ax,bx
        call      calcnewSF
eff_H_Vibrato_exit:
        ret
eff_H_Vibrato endp

eff_I_Tremor proc near
        ret
eff_I_Tremor endp

eff_J_Arpeggio proc near
        mov     bl,CHANOFF[TChannel.ArpegPos]
        xor     bh,bh
        inc     bx
        cmp     bx,3
        jb      inside
        xor     bx,bx
inside:
        mov     CHANOFF[TChannel.ArpegPos],bl
        shl     bx,2
        add     bp,bx
        mov     eax,CHANOFF[TChannel.step0]
        sub     bp,bx
        mov     CHANOFF[TChannel.sStep],eax
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
        cmp     byte ptr CHANOFF[TChannel.ctick],0
        jz      doretrigg
        dec     byte ptr CHANOFF[TChannel.ctick]
        jnz     eff_Q_Retrigger_exit
doretrigg:
        xor     eax,eax
        mov     CHANOFF[TChannel.sCurPos],eax
        mov     al,CHANOFF[TChannel.Parameter]
        and     al,0fh
        jz      eff_Q_Retrigger_exit
        mov     CHANOFF[TChannel.ctick],al
        mov     bx,CHANOFF[TChannel.cmd2nd]
        call    [eff_Q_Retrigger_tab+bx]
eff_Q_Retrigger_exit:
        ret
eff_Q_Retrigger endp

eff_Q_Retrigger_SlideDown proc near
        mov     cl,CHANOFF[TChannel.parameter]
        shr     cl,4
        mov     al,1
        shl     al,cl
        sub     CHANOFF[TChannel.bSampleVol],al
        jnc     eff_Q_Retrigger_SlideDown_exit
        mov     byte ptr CHANOFF[TChannel.bSampleVol],0
eff_Q_Retrigger_SlideDown_exit:
        ret
eff_Q_Retrigger_SlideDown endp

eff_Q_Retrigger_Use2div3 proc near
; (it's 5/8 in real life ;)
        mov      al,CHANOFF[TChannel.bSampleVol]
        mov      ah,al
        shl      al,2             ; al = 4*volume , ah = volume
        add      al,ah            ; al = 5*volume
        shr      al,3             ; al = 5*volume/8
        mov      CHANOFF[TChannel.bSampleVol],al
        ret
eff_Q_Retrigger_Use2div3 endp

eff_Q_Retrigger_Use1div2 proc near
        shr      byte ptr CHANOFF[TChannel.bSampleVol],1
        ret
eff_Q_Retrigger_Use1div2 endp

eff_Q_Retrigger_SlideUp proc near
        mov     cl,CHANOFF[TChannel.parameter]
        shr     cl,4
        mov     al,1
        shl     al,cl
        add     al,CHANOFF[TChannel.bSampleVol]
        call    voltest
        ret
eff_Q_Retrigger_SlideUp endp

eff_Q_Retrigger_Use3div2 proc near
        mov     al,CHANOFF[TChannel.bSampleVol]
        mov     ah,al
        add     al,al            ; al = 2*volume , ah = volume
        add     al,ah            ; al = 3*volume
        shr     al,1             ; al = 3*volume/2
        call    voltest
        ret
eff_Q_Retrigger_Use3div2 endp

eff_Q_Retrigger_Use2div1 proc near
        mov     al,CHANOFF[TChannel.bSampleVol]
        shl     al,1
        call    voltest
        ret
eff_Q_Retrigger_Use2div1 endp

eff_R_Tremolo proc near
        ; next position in table:
        mov       al,CHANOFF[TChannel.Parameter]
        mov       dl,al
        and       dl,0fh
        shr       al,4
        mov       bl,CHANOFF[TChannel.Tablepos]
        add       bl,al
        cmp       bl,64
        jb        endoftest2
        sub       bl,64
endoftest2:
        mov       CHANOFF[TChannel.Tablepos],bl
        xor       bh,bh
        add       bx,CHANOFF[TChannel.TrmTabOfs]
        mov       al,ds:[bx]
        imul      dl
        sar       ax,6
        mov       bl,CHANOFF[TChannel.oldvolume]
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
        mov       CHANOFF[TChannel.bSampleVol],bl
        ret
eff_R_Tremolo endp

eff_S_Special proc near
        mov     bx,CHANOFF[TChannel.cmd2nd]
        call    [eff_S_Special_tab+bx]
        ret
eff_S_Special endp

eff_S_Special_NoteCut proc near
        dec     byte ptr CHANOFF[TChannel.ndTick]
        jnz     eff_S_Special_NoteCut_exit
        mov     byte ptr CHANOFF[TChannel.bEnabled],0       ;disable it ...
eff_S_Special_NoteCut_exit:
        ret
eff_S_Special_NoteCut endp

eff_S_Special_NoteDelay proc near
        push    fs  ; segment to volumetable, but we may destroy it here ...
        dec     byte ptr CHANOFF[TChannel.ndTick]
        jnz     eff_S_Special_NoteDelay_exit
        ; Ok now we have to calc things for the new note/instr ...
        ; 1. Setup Instrument
        mov     si,bp
        mov     al,[channel.savInst+si]
        cmp     al,00
        je      nonewinst
        mov     [channel.InstrNo+si],al
        call    SetupNewInst
nonewinst:
        mov     al,[channel.savNote+si]
        cmp     al,0ffh
        je      no_newnote
        cmp     al,0feh
        jne     normal_note
        mov     byte ptr [channel.bEnabled+si],0     ; stop mixing
        jmp     no_newnote
normal_note:
        mov     byte ptr [channel.bEnabled+si],1     ; yo do mixing
        mov     [channel.Note+si],al
        call    SetNewNote
no_newnote:
        mov     al,[channel.savVol+si]
        cmp     al,0ffh
        je      no_vol
        mul     [gvolume]
        shr     ax,6
        mov     ds:[channel.bSampleVol+si],al
no_vol:
        mov     ds:[channel.command+si],0       ; <- no more Notedelay
eff_S_Special_NoteDelay_exit:
        pop     fs
        ret
eff_S_Special_NoteDelay endp


eff_U_FineVibrato proc near
        cmp     byte ptr CHANOFF[TChannel.bEnabled],0
        je      eff_U_FineVibrato_exit
        ; next position in table:
        mov     al,CHANOFF[TChannel.VibPara]
        mov     dl,al
        and     dl,0fh
        shr     al,4
        mov     bl,CHANOFF[TChannel.Tablepos]
        add     bl,al
        cmp     bl,64
        jb      f_endoftest
        sub     bl,64
f_endoftest:
        mov     CHANOFF[TChannel.Tablepos],bl
        xor     bh,bh
        add     bx,CHANOFF[TChannel.VibTabOfs]
        mov     al,ds:[bx]
        imul    dl
        sar     ax,8
        mov     bx,CHANOFF[TChannel.Oldperiod]
        add     ax,bx
        call    calcnewSF
eff_U_FineVibrato_exit:
        ret
eff_U_FineVibrato endp

MIXING_TEXT ends

end

.model large,pascal
.386

DGROUP group _DATA

include ..\dos\emstool.def
include s3mtypes.def
include s3mvars.def
include effvars.def
include mixvars.def
include effects.def
include mixer.def
include mixer_.def

; general

chnSetCommand macro command
mov word ptr [SI][TChannel.wCommand],command*2
endm

chnIfCommand macro command
cmp word ptr [SI][TChannel.wCommand],command*2
endm

regIfCommand macro reg, command
cmp reg,command*2
endm

chnSetSubCommand macro command
mov word ptr [SI][TChannel.wCommand2],command*2
endm

; Initialize effects

effInitCall macro address
        call    address
endm

ISTART_ macro name
name proc near
endm

effInitReturn macro value
        mov al,&value
        ret
endm

effInitReturnNothing macro
        ret
endm

IEND___ macro name
name endp
endm

;  Handle effects

effHandleCall macro address
        call    address
endm

HSTART_ macro name
name proc near
endm

effHandleReturn macro value
        mov al,&value
        ret
endm

effHandleReturnNothing macro
        ret
endm

HEND___ macro name
name endp
endm

; declared in readnote.pas
extrn set_tempo: far

_DATA segment word public use16 'DATA'

EFFIDX_NONE              equ 0
EFFIDX_A_SET_SPEED       equ 1
EFFIDX_B_JUMP            equ 2
EFFIDX_C_PATTERN_BREAK   equ 3
EFFIDX_D_VOLUME_SLIDE    equ 4
EFFIDX_E_PITCH_DOWN      equ 5
EFFIDX_F_PITCH_UP        equ 6
EFFIDX_G_PORTAMENTO      equ 7
EFFIDX_H_VIBRATO         equ 8
EFFIDX_I_TREMOR          equ 9
EFFIDX_J_ARPEGGIO        equ 10
EFFIDX_K_VIB_VOLSLIDE    equ 11
EFFIDX_L_PORTA_VOLSLIDE  equ 12
EFFIDX_M                 equ 13
EFFIDX_N                 equ 14
EFFIDX_O                 equ 15
EFFIDX_P                 equ 16
EFFIDX_Q_RETRIG_VOLSLIDE equ 17
EFFIDX_R_TREMOLO         equ 18
EFFIDX_S_SPECIAL         equ 19
EFFIDX_T_SET_TEMPO       equ 20
EFFIDX_U_FINE_VIBRATO    equ 21
EFFIDX_V_SET_GVOLUME     equ 22
EFFIDX_W                 equ 23
EFFIDX_X                 equ 24
EFFIDX_Y                 equ 25
EFFIDX_Z                 equ 26

; we have to init all the effects :
effInit_tab label word
        dw offset effInit_None
        dw offset effInit_A_SetSpeed
        dw offset effInit_B_JumpToOrder
        dw offset effInit_C_PatternBreak
        dw offset effInit_D_VolumeSlide
        dw offset effInit_E_PitchDown
        dw offset effInit_F_PitchUp
        dw offset effInit_G_Portamento
        dw offset effInit_H_Vibrato
        dw offset effInit_I_Tremor  ; can't get it - ugly implementation
        dw offset effInit_J_Arpeggio
        dw offset effInit_K_Vib_VolSlide
        dw offset effInit_L_Porta_VolSlide
        dw offset effInit_None
        dw offset effInit_None
        dw offset effInit_None
        dw offset effInit_None
        dw offset effInit_Q_Retrig_VolSlide
        dw offset effInit_R_Tremolo
        dw offset effInit_S_Special
        dw offset effInit_T_SetTempo
        dw offset effInit_H_Vibrato ; 'U' here equal to 'H' (Vibrato)
        dw offset effInit_V_SetGVolume

; Ok now some tables for multichoise effects (e.g. 'Syx' but also 'Dxy',E,F)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
effInit_S_Special_tab label word
        dw offset effInit_None                   ; S0? - nothin
        dw offset effInit_None                   ; set filter -> not implemented (by ST3)
        dw offset effInit_None                   ; set glissando -> not implemented (by ST3)
        dw offset effInit_None                   ; set finetune -> not here ! look special2
        dw offset effInit_S4_SetVibratoWaveform
        dw offset effInit_S5_SetTremoloWaveform
        dw offset effInit_None                   ; does not exist
        dw offset effInit_None                   ; does not exist
        dw offset effInit_None                   ; maybe later (it's E8x - panning )
        dw offset effInit_None                   ; does not exist
        dw offset effInit_None                   ; stereo control not implemented
        dw offset effInit_SB_PatternLoop
        dw offset effInit_SC_NoteCut
        dw offset effInit_SD_NoteDelay
        dw offset effInit_SE_PatternDelay
        dw offset effInit_None                   ; funkrepeat -> not implemented

EFFIDX_Q_RETRIG_VOLSLIDE_NONE     equ 0
EFFIDX_Q_RETRIG_VOLSLIDE_DOWN     equ 1
EFFIDX_Q_RETRIG_VOLSLIDE_USE2DIV3 equ 2
EFFIDX_Q_RETRIG_VOLSLIDE_USE1DIV2 equ 3
EFFIDX_Q_RETRIG_VOLSLIDE_UP       equ 4
EFFIDX_Q_RETRIG_VOLSLIDE_USE3DIV2 equ 5
EFFIDX_Q_RETRIG_VOLSLIDE_USE2DIV1 equ 6

effInit_Q_Retrig_VolSlide_tab label word
        dw offset effInit_Q_Retrig_VolSlide_None
        dw offset effInit_Q_Retrig_VolSlide_Down
        dw offset effInit_Q_Retrig_VolSlide_Down
        dw offset effInit_Q_Retrig_VolSlide_Down
        dw offset effInit_Q_Retrig_VolSlide_Down
        dw offset effInit_Q_Retrig_VolSlide_Down
        dw offset effInit_Q_Retrig_VolSlide_Use2Div3
        dw offset effInit_Q_Retrig_VolSlide_Use1Div2
        dw offset effInit_Q_Retrig_VolSlide_None
        dw offset effInit_Q_Retrig_VolSlide_Up
        dw offset effInit_Q_Retrig_VolSlide_Up
        dw offset effInit_Q_Retrig_VolSlide_Up
        dw offset effInit_Q_Retrig_VolSlide_Up
        dw offset effInit_Q_Retrig_VolSlide_Up
        dw offset effInit_Q_Retrig_VolSlide_Use3Div2
        dw offset effInit_Q_Retrig_VolSlide_Use2Div1

; and some effects we have to handle after reading vol/inst/note
; maybe refresh etc.
effHandle_tab label word
        dw offset effHandle_None
        dw offset effHandle_None
        dw offset effHandle_None
        dw offset effHandle_None
        dw offset effHandle_D_VolumeSlide
        dw offset effHandle_E_PitchDown
        dw offset effHandle_F_PitchUp
        dw offset effHandle_G_Portamento
        dw offset effHandle_H_Vibrato
        dw offset effHandle_None
        dw offset effHandle_J_Arpeggio
        dw offset effHandle_K_Vib_VolSlide
        dw offset effHandle_L_Porta_VolSlide
        dw offset effHandle_None
        dw offset effHandle_None
        dw offset effHandle_O_SetSampleOffset
        dw offset effHandle_None
        dw offset effHandle_None
        dw offset effHandle_R_Tremolo
        dw offset effHandle_S_Special
        dw offset effHandle_None
        dw offset effHandle_H_Vibrato  ; 'U' here equal to 'H' (vibrato)
        dw offset effHandle_None

      ; Ok now some tables for multichoise effects (e.g. 'Syx' but also 'Dxy',E,F)
      ; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
effHandle_D_VolumeSlide_tab label word
        dw offset effHandle_None
        dw offset effHandle_None
        dw offset effHandle_D_VolumeSlide_FineDown
        dw offset effHandle_D_VolumeSlide_FineUp

effHandle_E_PitchDown_tab label word
        dw offset effHandle_None
        dw offset effHandle_E_PitchDown_Fine
        dw offset effHandle_E_PitchDown_Extra

effHandle_F_PitchUp_tab label word
        dw offset effHandle_None
        dw offset effHandle_F_PitchUp_Fine
        dw offset effHandle_F_PitchUp_Extra

effHandle_S_Special_tab label word
        dw offset effHandle_None                  ; S0? - nothin
        dw offset effHandle_None                  ; set filter -> not implemented (by ST3)
        dw offset effHandle_None                  ; set glissando -> not implemented (by ST3)
        dw offset effHandle_S3_FineTune
        dw offset effHandle_None                  ; not here -> special 1
        dw offset effHandle_None                  ; not here -> special 1
        dw offset effHandle_None                  ; does not exist
        dw offset effHandle_None                  ; does not exist
        dw offset effHandle_None                  ; not here -> special 1
        dw offset effHandle_None                  ; does not exist
        dw offset effHandle_None                  ; stereo control -> not implemented (by ST3)
        dw offset effHandle_None                  ; not here -> special 1
        dw offset effHandle_None                  ; not here -> special 1
        dw offset effHandle_None                  ; not here -> special 1
        dw offset effHandle_SE_PatternDelay
        dw offset effHandle_None                  ; funkrepeat -> not implemented

; Play state
        playState_jumpToOrder_bFlag db ?
        playState_jumpToOrder_bPos  db ?
        playState_patBreak_bFlag    db ?
        playState_patBreak_bPos     db ?
        playState_patLoop_bNow      db ?
        playState_gVolume_bFlag     db ?
        playState_gVolume_bValue    db ?
        playState_patDelay_bNow     db ?
            ; call 'readnotes' inside a pattern delay,
            ; if then ignore all notes/inst/vol !
; now some variables I added after I found out those amazing things
; about pattern delay
; save effect,parameter for pattern delay
        chnState_patDelay_wCommandSaved     dw ?
        chnState_patDelay_bParameterSaved   db ?
; Channel state
        chnState_cur_bNote  db ?
            ; normaly it will be a copie of es:[di], but in
            ; patterndelay == 0 -> ignore note
        chnState_cur_bIns   db ?    ; the same thing for instrument
        chnState_cur_bVol   db ?    ; and for volume
; to save portamento values :
        chnState_porta_flag             db ?
        chnState_porta_wSmpPeriodOld    dw ?
        chnState_porta_dSmpStepOld      dd ?
; a little one for arpeggio :
        chnState_arp_bFlag  db ?

_DATA ends

READNOTE_TEXT segment word public use16 'CODE'
assume cs:READNOTE_TEXT,ds:DGROUP

CheckPara0 MACRO
local cp
           cmp      al,0
           jnz      cp
           mov      al,[SI][TChannel.bParameter]
cp:        mov      [SI][TChannel.bParameter],al
ENDM

CheckPara0not MACRO
local cp
              cmp      al,0
              je       cp
              mov      [SI][TChannel.bParameter],al
cp:
ENDM

; Out: dx:ax = pointer to pattern data
mapPatternData proc near dPatData: dword
        mov     dx,word ptr [dPatData][2]
        cmp     dx,0c000h
        jae     mapPatternData@_map
        mov     ax,word ptr [dPatData][0]
        jmp     mapPatternData@_exit
mapPatternData@_map:
        push    bp
        mov     ax,[patListEMHandle]
        push    ax      ; uint16 handle
        xor     dh,dh
        push    dx      ; uint16 logPage
        push    0       ; uint8 physPage
        call    EmsMap
        pop     bp
        test    al,al
        jnz     mapPatternData@_ok
        xor     ax,ax
        xor     dx,dx
        jmp     mapPatternData@_exit
mapPatternData@_ok:
        mov     ax,word ptr [dPatData][2]
        movzx   ax,ah
        and     al,03fh
        mul     [patListPatLength]  ; ax = partno * pattern length
        add     ax,word ptr [dPatData][0]
        mov     dx,[FrameSEG]
mapPatternData@_exit:
        ret
mapPatternData endp

; put next notes into channels
public readnewnotes
readnewnotes proc far
local chnCounter: byte
             mov     [playState_jumpToOrder_bFlag],0
             mov     [playState_patBreak_bFlag],0
             mov     [playState_gVolume_bFlag],0
             mov     [playState_patLoop_bNow],0
             mov     al,[patterndelay]
             test    al,al
             setnz   al
             mov     [playState_patDelay_bNow],al
             mov     al,[usedchannels]
             mov     [chnCounter],al
             ; now the segment of current pattern :
             movzx   bx,[curorder]
             mov     bl,[order+bx]
             cmp     bl,254
             jae     nextorder
             shl     bx,1
             mov     ax,[pattern+bx]
             or      ax,ax
             jz      nopatloop
        push    ax      ; seg(data)
        push    0       ; off(data)
        call    mapPatternData
        mov     bx,ax
        or      bx,dx
        jz      nopatloop
        mov     di,ax
        mov     es,dx
        ; get next postion in current pattern
        mov     al,[usedchannels]
        mov     ah,al
        shl     al,2
        add     al,ah        ; al = 5*[usedchannels]
        mov     ah,[curline]
        mul     ah
        add     di,ax
        ; ES:DI - pointer to current position in current pattern
             lea    si,[Channel]
chnLoop:     cmp     byte ptr [SI][TChannel.bChannelType],2
             ja      handlenothing           ; <- for adlib channels
             mov     [chnState_porta_flag],0      ; <- set Flag back
             ; ok first do read current note,inst,vol -> if in pattern delay then ignore them !
             mov     [chnState_cur_bNote],0ffh
             mov     [chnState_cur_bIns],00h
             mov     [chnState_cur_bVol] ,0ffh
             cmp     [playState_patDelay_bNow],1
             je      ignorethem
             mov     al,es:[di]
             mov     [chnState_cur_bNote],al
             mov     al,es:[di+1]
             mov     [chnState_cur_bIns],al
             mov     al,es:[di+2]
             mov     [chnState_cur_bVol],al
ignorethem:  ; read effects - it may change the read instr/note !
             ; ~~~~~~~~~~~~
             mov     al,es:[di+3]        ; read effect number
             xor     ah,ah
             shl     ax,1
             mov     byte ptr [SI][TChannel.bEffFlags],0
             regIfCommand ax,EFFIDX_H_VIBRATO
             je      checkifcontV
             regIfCommand ax,EFFIDX_K_VIB_VOLSLIDE
             je      checkifcontV
             regIfCommand ax,EFFIDX_U_FINE_VIBRATO
             je      checkifcontV
             ; -> no vibrato effect, so check if last row there was one ...
             ; if there was - do refresh dSmpStep
             chnIfCommand EFFIDX_H_VIBRATO
             je      Vibend
             chnIfCommand EFFIDX_U_FINE_VIBRATO
             je      Vibend
             chnIfCommand EFFIDX_K_VIB_VOLSLIDE
             jne     noVibend
             ; refresh frequency
vibend:      push    ax
             mov     ax,[SI][TChannel.wSmpPeriodOld]
             mov     [SI][TChannel.wSmpPeriod],ax
             cmp     ax,0
             je      novibcalc
             push    ax  ; uint16 wPeriod
             call    mixCalcSampleStep
             mov     [SI][word ptr TChannel.dSmpStep][0],ax
             mov     [SI][word ptr TChannel.dSmpStep][2],dx
novibcalc:   pop     ax
novibend:
             regIfCommand ax,EFFIDX_R_TREMOLO
             je      checkifcontTrm
             regIfCommand ax,EFFIDX_J_ARPEGGIO
             je      checkifcontA
             ; - currently no arpeggio, but arpeggio was last row ?
             ; if there was - set dSmpStep = dArpSmpSteps[0] (refresh frequ)
        chnIfCommand EFFIDX_J_ARPEGGIO
             jne     noarpegend
             ; arpeggio ends :
             push    ax
             mov     eax,[SI][TChannel.dArpSmpSteps]
             mov     [SI][TChannel.dSmpStep],eax
             pop     ax
noarpegend:
             regIfCommand ax,EFFIDX_Q_RETRIG_VOLSLIDE
             je      checkifcontRetr
aftercontcheck:
             mov     bx,[SI][TChannel.wCommand]
             mov     [chnState_patDelay_wCommandSaved],bx   ; to save it for pattern delay ...
             mov     [SI][TChannel.wCommand],ax
             chnSetSubCommand 0
             regIfCommand al,EFFIDX_V_SET_GVOLUME  ; last effect in table
             jbe     noproblem
        chnSetCommand EFFIDX_NONE
             xor     ax,ax
noproblem:   mov     bx,ax
        mov     al,es:[di+4]    ; read effect parameter
        effInitCall [effInit_tab+bx]
        test    al,al
        jz      handlenothing

effectdone:  ; read instrument
             ; ~~~~~~~~~~~~~~~
             ; reinit instrument data but don't restart !
             mov     al,[chnState_cur_bIns]
             cmp     al,00
             je      no_newinstr

             ; ------------------ check if instrument does exist
             push    ax
             dec     al
             xor     ah,ah
             mov     bx,ax
             shl     bx,2
             add     bx,ax
             mov     ax,word ptr ds:[Instruments+2]
             add     bx,ax
             mov     fs,bx
             cmp     fs:[TInstrument.typ],1
             je      instok
             pop     ax
             xor     al,al
             mov     [chnState_cur_bIns],al
             jmp     no_newinstr
instok:      pop     ax

comment #
    if ((!chn->bEnabled) || (chn->bIns != {al}))
    {
        if (!chnState_porta_flag)
            chn->dSmpPos = chn->wSmpStart << 16;
    };
#

        push    es  ; save
        push    ds  ; FP_SEG(channel_t *)
        push    si  ; FP_OFF(channel_t *)
        xor     ah,ah
        push    ax  ; uint8 insNum
        call    chn_setupInstrument
        pop     es  ; restore
no_newinstr: ; read note ...
             ; ~~~~~~~~~~~~~
             mov     al,[chnState_cur_bNote]
             cmp     al,0ffh
             je      no_newnote
             cmp     al,0feh
             jne     normal_note
             mov     byte ptr [SI][TChannel.bEnabled],0    ; stop mixing
             jmp     no_newnote
normal_note:
        push    es  ; save
        push    ds  ; FP_SEG(channel_t *)
        push    si  ; FP_OFF(channel_t *)
        xor     ah,ah
        push    ax  ; uint8 note
        mov     al,[chnState_porta_flag]
        push    ax  ; bool keep
        call    chn_setupNote
        pop     es  ; restore
no_newnote:  ; read volume - last but not least ;)
             ; ~~~~~~~~~~~
             mov     al,[chnState_cur_bVol]
             cmp     al,0ffh
             je      no_vol
             cmp     al,64
             jb      volok
             mov     al,63
volok:       mul     [gvolume]
             shr     ax,6
             mov     [SI][TChannel.bSmpVol],al

no_vol:
        ; ok now the effect handling after reading vol/instr/note
        mov bx,[SI][TChannel.wCommand]
         effHandleCall [effHandle_tab+bx]

handlenothing:
             add     di,5                ; to next channel in pattern
             add     si,size TChannel    ; to next channel in channel mix info
             dec     [chnCounter]        ; one channel done
             jnz     chnLoop
             cmp     [playState_gVolume_bFlag],0
             je      nonewglobal
             mov     al,[playState_gVolume_bValue]
             mov     [gvolume],al
nonewglobal: mov     al,[curspeed]
             mov     [curtick],al
             cmp     [playState_patBreak_bFlag],1
             je      patternbreak
             cmp     [playState_jumpToOrder_bFlag],1
             je      jumporder
             cmp     [playState_patLoop_bNow],1
             je      dopatloop

nopatloop:   inc     [curline]
             cmp     [curline],64
             jne     only_next
             ; ok - next order ...
             mov     [curline],0
nextorder:   inc     [curorder]
otherorder:  mov     [Ploop_to],0
             mov     al,[lastorder]
             cmp     [curorder],al
             ja      end_reached
doloop:
             movzx   bx,[curorder]
             mov     al,[Order+bx]
             cmp     al,254
             jae     nextorder
             mov     [curpattern],al
only_next:   ret

patternbreak:mov     al,[playState_patBreak_bPos]
             mov     [curline],al
             jmp     nextorder

jumporder:   mov     [curline],0
             mov     al,[playState_jumpToOrder_bPos]
             mov     [curorder],al
             jmp     otherorder

dopatloop:   dec     [Ploop_no]
             jz      patloopends
             mov     al,[Ploop_to]
             mov     [curline],al
             jmp     only_next
patloopends: mov     al,[curline]
             inc     al
             mov     [PLoop_to],al
             mov     [Ploop_on],0
             jmp     nopatloop

end_reached: cmp     [playOption_LoopSong],1
             jne     donotloop
             mov     [curorder],0
             jmp     doloop
donotloop:   mov     [EndOfSong],1
             jmp     only_next

checkifcontV:  ; check if continue Vib/effInit_K_Vib_VolSlide
             cmp     [chnState_cur_bNote],0feh
             jb      aftercontcheck
             chnIfCommand EFFIDX_K_VIB_VOLSLIDE  ; command before was vibrato
             je      checkok1
             chnIfCommand EFFIDX_U_FINE_VIBRATO  ; command before was fine vibrato
             je      checkok1
             chnIfCommand EFFIDX_H_VIBRATO   ; command before was vibrato+volef
             jne     aftercontcheck
checkok1:    mov     byte ptr [SI][TChannel.bEffFlags],EFFFLAG_CONTINUE
             jmp     aftercontcheck

checkifcontA:     ; check if continue Arpeg
             cmp     [chnState_cur_bNote],0feh
             jb      aftercontcheck
checkifcontTrm:   ; check if continue a tremolo
checkifcontRetr:  ; check if continue a note retrig
             cmp     [SI][TChannel.wCommand],ax
             jne     aftercontcheck
             mov     byte ptr [SI][TChannel.bEffFlags],EFFFLAG_CONTINUE
             jmp     aftercontcheck

readnewnotes endp

; Effects :
; ~~~~~~~~~

ISTART_ effInit_None
        checkPara0
        effInitReturn 1
IEND___ effInit_None

ISTART_ effInit_A_SetSpeed
        checkPara0not
        cmp     al,0    ; speed = 0 does not exist
        je      effInit_A_SetSpeed@_exit
        mov     [curspeed],al
effInit_A_SetSpeed@_exit:
        effInitReturn 1
IEND___ effInit_A_SetSpeed

ISTART_ effInit_B_JumpToOrder
        checkPara0not
        mov     [playState_jumpToOrder_bFlag],1
        mov     [playState_jumpToOrder_bPos],al
        effInitReturn 1
IEND___ effInit_B_JumpToOrder

ISTART_ effInit_C_PatternBreak
        checkPara0not
        mov     [playState_patBreak_bFlag],1
        mov     ah,al
        and     al,0fh
        shr     ah,4
        AAD
        mov     [playState_patBreak_bPos],al
        effInitReturn 1
IEND___ effInit_C_PatternBreak

ISTART_ effInit_D_VolumeSlide
               checkPara0
               mov      ah,al
               shr      ah,4
               cmp      ah,0fh
               jne      effInit_D_VolumeSlide@_noFineDown
               cmp      al,0f0h
               je       effInit_D_VolumeSlide@_up
               ; normal fine volume down   DFx
        chnSetSubCommand 2
        jmp effInit_D_VolumeSlide@_exit
effInit_D_VolumeSlide@_noFineDown:
               mov      ah,al
               and      ah,0fh
               cmp      ah,0fh
               jne      effInit_D_VolumeSlide@_noFineUp
               cmp      al,0fh
               je       effInit_D_VolumeSlide@_down
        ; normal fine volume up DxF
        chnSetSubCommand 3
        jmp effInit_D_VolumeSlide@_exit
effInit_D_VolumeSlide@_noFineUp:
               mov      ah,al
               and      ah,00fh
               jz       effInit_D_VolumeSlide@_up
effInit_D_VolumeSlide@_down:
        ; normal slide down effect D0x
        chnSetSubCommand 0
        jmp effInit_D_VolumeSlide@_exit
effInit_D_VolumeSlide@_up:
        ; normal slide up effect Dx0
        chnSetSubCommand 1
effInit_D_VolumeSlide@_exit:
        effInitReturn 1
IEND___ effInit_D_VolumeSlide

ISTART_ effInit_E_PitchDown
               checkPara0
               cmp      al,0DFh      ; al is parameter
               ja       effInit_E_PitchDown@_extra
        chnSetSubCommand 0
        jmp effInit_E_PitchDown@_exit
effInit_E_PitchDown@_extra:
               cmp      al,0EFh
               ja       effInit_E_PitchDown@_fine
               ; extra fine slides
        chnSetSubCommand 2
        jmp effInit_E_PitchDown@_exit
effInit_E_PitchDown@_fine:
        chnSetSubCommand 1
effInit_E_PitchDown@_exit:
        effInitReturn 1
IEND___ effInit_E_PitchDown

ISTART_ effInit_F_PitchUp
               checkPara0
               cmp      al,0DFh      ; al is parameter
               ja       effInit_F_PitchUp@_extra
        chnSetSubCommand 0
        jmp effInit_F_PitchUp@_exit
effInit_F_PitchUp@_extra:
               cmp      al,0EFh
               ja       effInit_F_PitchUp@_fine
               ; extra fine slides
        chnSetSubCommand 2
        jmp effInit_F_PitchUp@_exit
effInit_F_PitchUp@_fine:
        chnSetSubCommand 1
effInit_F_PitchUp@_exit:
        effInitReturn 1
IEND___ effInit_F_PitchUp

ISTART_ effInit_G_Portamento_Stop
        mov     [chnState_porta_flag],0
        chnSetCommand EFFIDX_NONE
        effInitReturnNothing
IEND___ effInit_G_Portamento_Stop

ISTART_ effInit_G_Portamento
               checkPara0not
               cmp      al,0
               je       effInit_G_Portamento@_noNewParam
               mov      [SI][TChannel.bPortParam],al
effInit_G_Portamento@_noNewParam:
               mov      [chnState_porta_flag],1
               ; check first if portamento really possible:
               cmp      byte ptr [SI][TChannel.bEnabled],0
               je       effInit_G_Portamento@_stop
               cmp      [chnState_cur_bNote],0feh
               jae      effInit_G_Portamento@_exit  ; <- continue portamento
               ; now save some values (we wanna slide from ...)
               mov      eax,[SI][TChannel.dSmpStep]
               mov      [chnState_porta_dSmpStepOld],eax
               mov      ax,[SI][TChannel.wSmpPeriod]
               mov      [chnState_porta_wSmpPeriodOld],ax
        jmp effInit_G_Portamento@_exit
effInit_G_Portamento@_stop:
        effInitCall effInit_G_Portamento_Stop
effInit_G_Portamento@_exit:
        effInitReturn 1
IEND___ effInit_G_Portamento

ISTART_ effInit_H_Vibrato
               checkpara0not
               test     byte ptr [SI][TChannel.bEffFlags],EFFFLAG_CONTINUE
               jnz      effInit_H_Vibrato@_noRestart
               mov      byte ptr [SI][TChannel.bTabPos],0
effInit_H_Vibrato@_noRestart:
               cmp      al,0
               jne      effInit_H_Vibrato@_newValue
               mov      al,[SI][TChannel.bVibParam]
effInit_H_Vibrato@_newValue:
               mov      ah,al
               shr      ah,4
               cmp      ah,0
               jne      effInit_H_Vibrato@_speedOk
               mov      ah,[SI][TChannel.bVibParam]
               and      ah,0f0h
               or       al,ah
effInit_H_Vibrato@_speedOk:
               mov      [SI][TChannel.bVibParam],al
        effInitReturn 1
IEND___ effInit_H_Vibrato

ISTART_ effInit_I_Tremor
               checkPara0
        effInitReturn 1
IEND___ effInit_I_Tremor

ISTART_ effInit_J_Arpeggio
               cmp      al,0
               je       effInit_J_Arpeggio@_useLastPara
               mov      [chnState_arp_bFlag],1
               mov      [SI][TChannel.bParameter],al
        jmp effInit_J_Arpeggio@_exit
effInit_J_Arpeggio@_useLastPara:
               mov      [chnState_arp_bFlag],0
effInit_J_Arpeggio@_exit:
        effInitReturn 1
IEND___ effInit_J_Arpeggio

ISTART_ effInit_K_Vib_VolSlide
               test     byte ptr [SI][TChannel.bEffFlags],EFFFLAG_CONTINUE
               jnz      effInit_K_Vib_VolSlide@_volumeSlide
               mov      byte ptr [SI][TChannel.bTabPos],0
effInit_K_Vib_VolSlide@_volumeSlide:
        effInitCall effInit_D_VolumeSlide
        effInitReturn 1
IEND___ effInit_K_Vib_VolSlide

ISTART_ effInit_L_Porta_VolSlide
               mov      [chnState_porta_flag],1
               ; check first if portamento really possible:
               cmp      byte ptr [SI][TChannel.bEnabled],0
               je       effInit_L_Porta_VolSlide@_stop  ; <- channel plays nothing -> no porta and volfx usefull ;)
               cmp      [chnState_cur_bNote],0feh
               jae      effInit_L_Porta_VolSlide@_volumeSlide   ; <- continue portamento
               ; now save some values (we wanna slide from ...)
               push     ax
               mov      eax,[SI][TChannel.dSmpStep]
               mov      [chnState_porta_dSmpStepOld],eax
               mov      ax,[SI][TChannel.wSmpPeriod]
               mov      [chnState_porta_wSmpPeriodOld],ax
               pop      ax
effInit_L_Porta_VolSlide@_volumeSlide:
        effInitCall effInit_D_VolumeSlide
        jmp effInit_L_Porta_VolSlide@_exit
effInit_L_Porta_VolSlide@_stop:
        effInitCall effInit_G_Portamento_Stop
effInit_L_Porta_VolSlide@_exit:
        effInitReturn 1
IEND___ effInit_L_Porta_VolSlide

ISTART_ effInit_Q_Retrig_VolSlide
               cmp      al,0
               jne      newretrPara
               mov      al,[SI][TChannel.bParameter]
               jmp      aftersetretr
newretrPara:   mov      [SI][TChannel.bParameter],al
               mov      ah,al
               and      ah,0fh
               cmp      ah,0
               je       noretrigg
               dec      ah
               mov      [SI][TChannel.bRetrigTicks],ah
aftersetretr:
        movzx   bx,al
        shr     bx,4
        shl     bx,1
        effInitCall [effInit_Q_Retrig_VolSlide_tab+bx]
        effInitReturnNothing
IEND___ effInit_Q_Retrig_VolSlide

ISTART_ effInit_Q_Retrig_VolSlide_None
        chnSetSubCommand EFFIDX_Q_RETRIG_VOLSLIDE_NONE
        effInitReturn 1
IEND___ effInit_Q_Retrig_VolSlide_None

ISTART_ effInit_Q_Retrig_VolSlide_Down
        chnSetSubCommand EFFIDX_Q_RETRIG_VOLSLIDE_DOWN
        effInitReturn 1
IEND___ effInit_Q_Retrig_VolSlide_Down

ISTART_ effInit_Q_Retrig_VolSlide_Use2Div3
        chnSetSubCommand EFFIDX_Q_RETRIG_VOLSLIDE_USE2DIV3
        effInitReturn 1
IEND___ effInit_Q_Retrig_VolSlide_Use2Div3

ISTART_ effInit_Q_Retrig_VolSlide_Use1Div2
        chnSetSubCommand EFFIDX_Q_RETRIG_VOLSLIDE_USE1DIV2
        effInitReturn 1
IEND___ effInit_Q_Retrig_VolSlide_Use1Div2

ISTART_ effInit_Q_Retrig_VolSlide_Up
        chnSetSubCommand EFFIDX_Q_RETRIG_VOLSLIDE_UP
        effInitReturn 1
IEND___ effInit_Q_Retrig_VolSlide_Up

ISTART_ effInit_Q_Retrig_VolSlide_Use3Div2
        chnSetSubCommand EFFIDX_Q_RETRIG_VOLSLIDE_USE3DIV2
        effInitReturn 1
IEND___ effInit_Q_Retrig_VolSlide_Use3Div2

ISTART_ effInit_Q_Retrig_VolSlide_Use2Div1
        chnSetSubCommand EFFIDX_Q_RETRIG_VOLSLIDE_USE2DIV1
        effInitReturn 1
IEND___ effInit_Q_Retrig_VolSlide_Use2Div1

ISTART_ noretrigg
        chnSetCommand EFFIDX_NONE
        effInitReturn 1
IEND___ noretrigg

ISTART_ effInit_R_Tremolo
               test     byte ptr [SI][TChannel.bEffFlags],EFFFLAG_CONTINUE
               jnz      noTrmrestart
               mov      byte ptr [SI][TChannel.bTabPos],0
noTrmrestart:
               cmp      al,0
               jne      newTrmvalue
               mov      al,[SI][TChannel.bParameter]
newTrmValue:
               mov      ah,al
               shr      ah,4
               cmp      ah,0
               jne      Trmspeedok
               mov      ah,[SI][TChannel.bParameter]
               and      ah,0f0h
               or       al,ah
TrmspeedOk:
               mov     [SI][TChannel.bParameter],al
        effInitReturn 1
IEND___ effInit_R_Tremolo

ISTART_ effInit_S_Special
               checkPara0
               xor      bh,bh
               mov      bl,al
               shr      bl,4
               shl      bx,1
               mov      [SI][TChannel.wCommand2],bx
        ; do effects we have to check before reading note/vol/instr
        effInitCall [effInit_S_Special_tab+bx]
        effInitReturnNothing
IEND___ effInit_S_Special

ISTART_ effInit_S4_SetVibratoWaveform
        ;for every channel a seperate choise !
               and      al,3
               add      al,al
               xor      ah,ah
               mov      bx,ax
               shl      bx,1
               mov      ax,[wavetab+bx]
               mov      [SI][TChannel.wVibTab],ax
        effInitReturn 1
IEND___ effInit_S4_SetVibratoWaveform

ISTART_ effInit_S5_SetTremoloWaveform
        ; for every channel a seperate choise !
               and      al,3
               add      al,al
               xor      ah,ah
               mov      bx,ax
               shl      bx,1
               mov      ax,[wavetab+bx]
               mov      [SI][TChannel.wTrmTab],ax
        effInitReturn 1
IEND___ effInit_S5_SetTremoloWaveform

ISTART_ effInit_SB_PatternLoop
               and      al,0fh
               cmp      al,0
               je       set2where
               cmp      [Ploop_on],1
               je       alreadyinside
               mov      [Ploop_on],1
               inc      al
               mov      [Ploop_no],al
alreadyinside:
               mov      [playState_patLoop_bNow],1
        jmp effInit_SB_PatternLoop@_exit
set2where:
               mov      al,[curline]
               mov      [Ploop_to],al
effInit_SB_PatternLoop@_exit:
        effInitReturn 1
IEND___ effInit_SB_PatternLoop

ISTART_ effInit_SC_NoteCut
               and      al,0fh
               cmp      al,0
               je       effInit_SC_NoteCut@_noCut
               ;inc      al
               mov      [SI][TChannel.bDelayTicks],al
        jmp effInit_SC_NoteCut@_exit
effInit_SC_NoteCut@_noCut:
        chnSetCommand 0	; FIXME: 9 = mistake? Zero?
effInit_SC_NoteCut@_exit:
        effInitReturn 1
IEND___ effInit_SC_NoteCut

ISTART_ effInit_SD_NoteDelay
               and      al,0fh
               mov      [SI][TChannel.bDelayTicks],al
               cmp      [playState_patDelay_bNow],1
               je       effInit_SD_NoteDelay@_exit
               mov      al,[chnState_cur_bNote]      ; new note for later
               mov      [SI][TChannel.bSavNote],al
               mov      al,[chnState_cur_bIns]    ; new instrument for later
               mov      [SI][TChannel.bSavIns],al
               mov      al,[chnState_cur_bVol]    ; new volume for later
               mov      [SI][TChannel.bSavVol],al
effInit_SD_NoteDelay@_exit:
        ; setup note/instr/vol later while mixing !
        effInitReturn 0
IEND___ effInit_SD_NoteDelay

ISTART_ effInit_SE_PatternDelay
        cmp [byte ptr playState_patDelay_bNow],0
        jne  effInit_SE_PatternDelay@_exit  ; we are already in this pattern delay
               and      al,0fh
               inc      al
               mov      [patterndelay],al
               mov      al,[SI][TChannel.bParameter]
               mov      [chnState_patDelay_bParameterSaved],al
effInit_SE_PatternDelay@_exit:
        effInitReturn 1
IEND___ effInit_SE_PatternDelay

ISTART_ effInit_T_SetTempo
        checkPara0not
        xor     ah,ah
        push    si
        push    di
        push    fs
        push    es
        push    ax  ; arg
        call    set_tempo
        pop     es
        pop     fs
        pop     di
        pop     si
        effInitReturn 1
IEND___ effInit_T_SetTempo

ISTART_ effInit_V_SetGVolume
               checkPara0not
               mov      [playState_gVolume_bFlag],1
               cmp      al,64
               jbe      effInit_V_SetGVolume@_volok
               mov      al,64
effInit_V_SetGVolume@_volok:
               mov      [playState_gVolume_bValue],al
        effInitReturn 1
IEND___ effInit_V_SetGVolume

;  _____________________________________
;  Effect after reading vol/instr/note :
;  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

HSTART_ effHandle_None
        effHandleReturnNothing
HEND___ effHandle_None

HSTART_ effHandle_D_VolumeSlide
        mov bx,[SI][TChannel.wCommand2]
        effHandleCall [effHandle_D_VolumeSlide_tab+bx]
        effHandleReturnNothing
HEND___ effHandle_D_VolumeSlide

HSTART_ effHandle_D_VolumeSlide_FineDown
        mov     al,[SI][TChannel.bSmpVol]
        mov     ah,[SI][TChannel.bParameter]
        and     ah,0fh
        sub     al,ah
        jnc     effHandle_D_VolumeSlide_FineDown@_ok
        xor     al,al
effHandle_D_VolumeSlide_FineDown@_ok:
        mov     byte ptr [SI][TChannel.bSmpVol],al
        effHandleReturnNothing
HEND___ effHandle_D_VolumeSlide_FineDown

HSTART_ effHandle_D_VolumeSlide_FineUp
        mov     al,[SI][TChannel.bSmpVol]
        mov     ah,[SI][TChannel.bParameter]
        shr     ah,4
        add     al,ah
        cmp     al,64
        jb      effHandle_D_VolumeSlide_FineUp@_ok
        mov     al,63
effHandle_D_VolumeSlide_FineUp@_ok:
        mov     byte ptr [SI][TChannel.bSmpVol],al
        effHandleReturnNothing
HEND___ effHandle_D_VolumeSlide_FineUp

HSTART_ effHandle_E_PitchDown
        mov bx,[SI][TChannel.wCommand2]
        effHandleCall [effHandle_E_PitchDown_tab+bx]
        effHandleReturnNothing
HEND___ effHandle_E_PitchDown

HSTART_ ptok
; in: ax = period
;     ds:si = mixchn
        ; now calc new frequency step for this period
        mov     [SI][TChannel.wSmpPeriod],ax
        push    ax  ; uint16 wPeriod
        call    mixCalcSampleStep
        mov     [SI][word ptr TChannel.dSmpStep][0],ax
        mov     [SI][word ptr TChannel.dSmpStep][2],dx
        effHandleReturnNothing
HEND___ ptok

HSTART_ effHandle_E_PitchDown_Fine
              ; we pitch down, but increase period ! (so check wSmpPeriodHigh)
              mov       ax,[SI][TChannel.wSmpPeriod]
              mov       bl,[SI][TChannel.bParameter]
              and       bl,0fh
              xor       bh,bh
              shl       bx,2
              add       ax,bx
              cmp       ax,[SI][TChannel.wSmpPeriodHigh]
              jb        effHandle_E_PitchDown_Fine@_ptok
              mov       ax,[SI][TChannel.wSmpPeriodHigh]
effHandle_E_PitchDown_Fine@_ptok:
        effHandleCall ptok
        effHandleReturnNothing
HEND___ effHandle_E_PitchDown_Fine

HSTART_ effHandle_E_PitchDown_Extra
              ; we pitch down, but increase period ! (so check wSmpPeriodHigh)
              mov       ax,[SI][TChannel.wSmpPeriod]
              mov       bl,[SI][TChannel.bParameter]
              and       bl,0fh
              xor       bh,bh
              add       ax,bx
              cmp       ax,[SI][TChannel.wSmpPeriodHigh]
              jb        effHandle_E_PitchDown_Extra@_ptok
              mov       ax,[SI][TChannel.wSmpPeriodHigh]
effHandle_E_PitchDown_Extra@_ptok:
        effHandleCall ptok
        effHandleReturnNothing
HEND___ effHandle_E_PitchDown_Extra

HSTART_ effHandle_F_PitchUp
        mov bx,[SI][TChannel.wCommand2]
        effHandleCall [effHandle_F_PitchUp_tab+bx]
        effHandleReturnNothing
HEND___ effHandle_F_PitchUp

HSTART_ effHandle_F_PitchUp_Fine
              ; we pitch up, but decrease period ! (so check wSmpPeriodLow)
              mov       ax,[SI][TChannel.wSmpPeriod]
              mov       bl,[SI][TChannel.bParameter]
              and       bl,0fh
              xor       bh,bh
              shl       bx,2
              sub       ax,bx
              cmp       ax,[SI][TChannel.wSmpPeriodLow]
              ja        effHandle_F_PitchUp_Fine@_ptok
              mov       ax,[SI][TChannel.wSmpPeriodLow]
effHandle_F_PitchUp_Fine@_ptok:
        effHandleCall ptok
        effHandleReturnNothing
HEND___ effHandle_F_PitchUp_Fine

HSTART_ effHandle_F_PitchUp_Extra
              ; we pitch up, but decrease period ! (so check wSmpPeriodLow)
              mov       ax,[SI][TChannel.wSmpPeriod]
              mov       bl,[SI][TChannel.bParameter]
              and       bl,0fh
              xor       bh,bh
              sub       ax,bx
              cmp       ax,[SI][TChannel.wSmpPeriodLow]
              ja        effHandle_F_PitchUp_Extra@_ptok
              mov       ax,[SI][TChannel.wSmpPeriodLow]
effHandle_F_PitchUp_Extra@_ptok:
        effHandleCall ptok
        effHandleReturnNothing
HEND___ effHandle_F_PitchUp_Extra

HSTART_ effHandle_G_Portamento
        cmp     [chnState_cur_bNote],0feh
        jae     effHandle_G_Portamento@_exit ; <- is a portamento continue
        mov     ax,[chnState_porta_wSmpPeriodOld]
        xchg    ax,[SI][TChannel.wSmpPeriod]
        mov     [SI][TChannel.wSmpPeriodDest],ax
        mov     eax,[chnState_porta_dSmpStepOld]
        mov     [SI][TChannel.dSmpStep],eax
effHandle_G_Portamento@_exit:
        effHandleReturnNothing
HEND___ effHandle_G_Portamento

HSTART_ effHandle_H_Vibrato
               test     byte ptr [SI][TChannel.bEffFlags],EFFFLAG_CONTINUE
               jnz      effHandle_H_Vibrato@_exit ; continue this effect !
               mov      ax,[SI][TChannel.wSmpPeriod]
               mov      [SI][TChannel.wSmpPeriodOld],ax
effHandle_H_Vibrato@_exit:
        effHandleReturnNothing
HEND___ effHandle_H_Vibrato

HSTART_ effHandle_J_Arpeggio
               cmp      [byte ptr chnState_arp_bFlag],0
               jne      newPara
               test     byte ptr [SI][TChannel.bEffFlags],EFFFLAG_CONTINUE
               jnz      effHandle_J_Arpeggio@_exit ; no new note !
               ; start arpeggio:
               mov      byte ptr [SI][TChannel.bArpPos],0
newPara:       mov      al,[SI][TChannel.bParameter]
               mov      ah,[SI][TChannel.bNote]
               ; calc note 1:
               mov      bl,ah
               mov      bh,al
               shr      bh,4            ; upper 4 bits for note 1
               add      bl,bh
               mov      bh,bl
               and      bh,0f0h
               and      bl,00fh
arpt1:         cmp      bl,12
               jb       arpok1
               sub      bl,12
               add      bh,10h
               jmp      arpt1
arpok1:        or       bh,bl
               mov      [SI][TChannel.bArpNotes][0],bh
               ; now note 2:
               mov      bl,ah
               mov      bh,al
               and      bh,0fh          ; lower 4 bits for note 2
               add      bl,bh
               mov      bh,bl
               and      bh,0f0h
               and      bl,00fh
arpt2:         cmp      bl,12
               jb       arpok2
               sub      bl,12
               add      bh,10h
               jmp      arpt2
arpok2:        or       bh,bl
               mov      [SI][TChannel.bArpNotes][1],bh
               ; now calc the Steps:
               mov     ax,[SI][TChannel.wInsSeg]
               mov     fs,ax
               mov      al,[SI][TChannel.bNote]
               push     ds  ; FP_SEG(chn)
               push     si  ; FP_OFF(chn)
               push     fs  ; FP_SEG(ins)
               xor      bx,bx
               push     bx  ; FP_OFF(ins)
               push     ax  ; uint8 note
               call    chn_calcNoteStep;calcpart
               mov      [SI][word ptr TChannel.dArpSmpSteps][4*0][0],ax
               mov      [SI][word ptr TChannel.dArpSmpSteps][4*0][2],dx
               mov      al,[SI][TChannel.bArpNotes]
               push     ds  ; FP_SEG(chn)
               push     si  ; FP_OFF(chn)
               push     fs  ; FP_SEG(ins)
               xor      bx,bx
               push     bx  ; FP_OFF(ins)
               push     ax  ; uint8 note
               call    chn_calcNoteStep;calcpart
               mov      [SI][word ptr TChannel.dArpSmpSteps][4*1][0],ax
               mov      [SI][word ptr TChannel.dArpSmpSteps][4*1][2],dx
               mov      al,[SI][TChannel.bArpNotes+1]
               push     ds  ; FP_SEG(chn)
               push     si  ; FP_OFF(chn)
               push     fs  ; FP_SEG(ins)
               xor      bx,bx
               push     bx  ; FP_OFF(ins)
               push     ax  ; uint8 note
               call    chn_calcNoteStep;calcpart
               mov      [SI][word ptr TChannel.dArpSmpSteps][4*2][0],ax
               mov      [SI][word ptr TChannel.dArpSmpSteps][4*2][2],dx
effHandle_J_Arpeggio@_exit:
        effHandleReturnNothing
HEND___ effHandle_J_Arpeggio

HSTART_ effHandle_K_Vib_VolSlide
               test     [SI][TChannel.bEffFlags],EFFFLAG_CONTINUE
               jnz      effHandle_K_Vib_VolSlide@_volfx
               mov      ax,[SI][TChannel.wSmpPeriod]
               mov      [SI][TChannel.wSmpPeriodOld],ax
effHandle_K_Vib_VolSlide@_volfx:
        effHandleCall effHandle_D_VolumeSlide
        effHandleReturnNothing
HEND___ effHandle_K_Vib_VolSlide

HSTART_ effHandle_L_Porta_VolSlide
               ; first handle Portamento (and then jump to volume effect handling)
               cmp      [chnState_cur_bNote],0feh
               jae      effHandle_L_Porta_VolSlide@_volfx
               mov      ax,[chnState_porta_wSmpPeriodOld]
               xchg     ax,[SI][TChannel.wSmpPeriod]
               mov      [SI][TChannel.wSmpPeriodDest],ax
               mov      eax,[chnState_porta_dSmpStepOld]
               mov      [SI][TChannel.dSmpStep],eax
effHandle_L_Porta_VolSlide@_volfx:
        effHandleCall effHandle_D_VolumeSlide
        effHandleReturnNothing
HEND___ effHandle_L_Porta_VolSlide

HSTART_ effHandle_O_SetSampleOffset
               xor       eax,eax
               mov       ah,[SI][TChannel.bParameter]
               mov       [SI][TChannel.wSmpStart],ax
               cmp       [byte ptr chnState_cur_bNote],0ffh
               je        effHandle_O_SetSampleOffset@_exit
               shl       eax,16
               mov       [SI][TChannel.dSmpPos],eax
effHandle_O_SetSampleOffset@_exit:
        effHandleReturnNothing
HEND___ effHandle_O_SetSampleOffset

HSTART_ effHandle_R_Tremolo
               cmp      [byte ptr chnState_cur_bIns],0
               jne      savevol                      ; new instrument
               cmp      [byte ptr chnState_cur_bVol],0ffh
               jne      savevol                      ; new volume
               test     [SI][TChannel.bEffFlags],EFFFLAG_CONTINUE
               jnz      effHandle_R_Tremolo@_exit ; continue this effect !
savevol:
              mov       al,[SI][TChannel.bSmpVol]
              mov       [SI][TChannel.bSmpVolOld],al
effHandle_R_Tremolo@_exit:
        effHandleReturnNothing
HEND___ effHandle_R_Tremolo

HSTART_ effHandle_S_Special
        mov bx,[SI][TChannel.wCommand2]
        effHandleCall [effHandle_S_Special_tab+bx]
        effHandleReturnNothing
HEND___ effHandle_S_Special

HSTART_ effHandle_S3_FineTune
        effHandleReturnNothing
HEND___ effHandle_S3_FineTune

HSTART_ effHandle_SE_PatternDelay
                mov       al,[chnState_patDelay_bParameterSaved]
                mov       [SI][TChannel.bParameter],al
                mov       ax,[chnState_patDelay_wCommandSaved]
                mov       [SI][TChannel.wCommand],ax
        chnSetSubCommand 0
        effHandleReturnNothing
HEND___ effHandle_SE_PatternDelay

READNOTE_TEXT ends

end

.model large,pascal
.386

DGROUP group _DATA

include ..\dos\emstool.def
include s3mtypes.def
include s3mvars.def
include effvars.def
include mixvars.def
include mixer.def
include mixer_.def
include effects.def

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

_DATA segment word public use16 'DATA'
_DATA ends

READNOTE_TEXT segment word public use16 'CODE'
assume cs:READNOTE_TEXT,ds:DGROUP

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
        push    es  ; save
        push    di  ; save
        push    ds  ; FP_SEG(chn)
        push    si  ; FP_OFF(chn)
        xor     ah,ah
        push    ax  ; uint8 param
        call    chn_effInit
        pop     di  ; restore
        pop     es  ; restore
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
        push    es  ; save
        push    di  ; save
        push    ds  ; FP_SEG(chn)
        push    si  ; FP_OFF(chn)
        call    chn_effHandle
        pop     di  ; restore
        pop     es  ; restore

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

READNOTE_TEXT ends

end

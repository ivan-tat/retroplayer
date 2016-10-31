.387
		PUBLIC	__U8DQ
		PUBLIC	__U8DR
		PUBLIC	__I8DQ
		PUBLIC	__I8DR
		PUBLIC	__U8DQE
		PUBLIC	__U8DRE
		PUBLIC	__I8DQE
		PUBLIC	__I8DRE
DGROUP		GROUP	_DATA
I8D086_TEXT		SEGMENT	WORD PUBLIC USE16 'CODE'
		ASSUME CS:I8D086_TEXT, DS:DGROUP, SS:DGROUP
L$1:
    mov         word ptr ss:[bx],0 
    mov         word ptr ss:2[bx],0 
    mov         word ptr ss:4[bx],0 
    mov         word ptr ss:6[bx],0 
    mov         word ptr ss:8[bx],0 
    ret         
L$2:
    push        ds 
    push        es 
    push        ss 
    pop         es 
    push        ss 
    pop         ds 
    movsw       
    movsw       
    movsw       
    movsw       
    pop         es 
    pop         ds 
    ret         
L$3:
    xor         ax,ax 
    or          ax,word ptr ss:8[bx] 
    jne         L$5 
    or          ax,word ptr ss:6[bx] 
    jne         L$6 
    or          ax,word ptr ss:4[bx] 
    jne         L$7 
    or          ax,word ptr ss:2[bx] 
    jne         L$8 
    or          ax,word ptr ss:[bx] 
    jne         L$9 
L$4:
    inc         al 
    ret         
L$5:
    mov         al,9 
    jmp         L$10 
L$6:
    mov         al,7 
    jmp         L$10 
L$7:
    mov         al,5 
    jmp         L$10 
L$8:
    mov         al,3 
    jmp         L$10 
L$9:
    mov         al,1 
L$10:
    test        ah,ah 
    jne         L$4 
    ret         
L$11:
    mov         word ptr -3eH[bp],ax 
    mov         bx,di 
    call        near ptr L$1 
    lea         bx,7[di] 
    dec         di 
    xor         cx,cx 
L$12:
    xor         ah,ah 
    mov         al,byte ptr ss:[si] 
    inc         si 
    test        ax,ax 
    je          L$13 
    mul         word ptr -3eH[bp] 
L$13:
    add         ax,cx 
    inc         di 
    mov         byte ptr ss:[di],al 
    mov         cl,ah 
    cmp         di,bx 
    jne         L$12 
    inc         di 
    mov         byte ptr ss:[di],cl 
    ret         
L$14:
    mov         word ptr -40H[bp],ax 
    mov         bx,di 
    call        near ptr L$1 
    xor         dl,dl 
    lea         di,8[di] 
    lea         si,8[si] 
L$15:
    mov         ah,dl 
    dec         si 
    xor         dx,dx 
    mov         al,byte ptr ss:[si] 
    test        ax,ax 
    je          L$16 
    div         word ptr -40H[bp] 
L$16:
    dec         di 
    mov         byte ptr ss:[di],al 
    cmp         bx,di 
    jne         L$15 
    ret         
L$17:
    mov         si,word ptr -3cH[bp] 
    add         si,word ptr -36H[bp] 
    xor         dx,dx 
    mov         ax,word ptr -34H[bp+si] 
    mov         dl,byte ptr -32H[bp+si] 
    mov         si,word ptr -36H[bp] 
    div         word ptr -20H[bp+si] 
    test        ax,0ff00H 
    je          L$18 
    mov         ax,0ffH 
L$18:
    ret         
L$19:
    mov         di,word ptr -36H[bp] 
L$20:
    test        di,di 
    je          L$21 
    mov         si,word ptr -3cH[bp] 
    add         si,di 
    mov         al,byte ptr -28H[bp+di] 
    sub         al,byte ptr -32H[bp+si] 
    jne         L$22 
    dec         di 
    jmp         L$20 
L$21:
    mov         si,word ptr -3cH[bp] 
    add         si,di 
    mov         al,byte ptr -28H[bp+di] 
    sub         al,byte ptr -32H[bp+si] 
L$22:
    ret         
L$23:
    xor         di,di 
    xor         cl,cl 
L$24:
    cmp         di,word ptr -36H[bp] 
    jg          L$25 
    mov         si,word ptr -3cH[bp] 
    add         si,di 
    xor         ch,ch 
    xor         ax,ax 
    mov         al,byte ptr -32H[bp+si] 
    sub         ch,cl 
    sbb         al,byte ptr -28H[bp+di] 
    mov         byte ptr -32H[bp+si],al 
    sbb         cx,cx 
    inc         di 
    neg         cx 
    jmp         L$24 
L$25:
    ret         
L$26:
    mov         ax,word ptr ss:[di] 
    sub         ax,word ptr ss:[si] 
    mov         word ptr ss:[di],ax 
    mov         ax,word ptr ss:2[di] 
    sbb         ax,word ptr ss:2[si] 
    mov         word ptr ss:2[di],ax 
    mov         ax,word ptr ss:4[di] 
    sbb         ax,word ptr ss:4[si] 
    mov         word ptr ss:4[di],ax 
    mov         ax,word ptr ss:6[di] 
    sbb         ax,word ptr ss:6[si] 
    mov         word ptr ss:6[di],ax 
    mov         ax,word ptr ss:8[di] 
    sbb         ax,word ptr ss:8[si] 
    mov         word ptr ss:8[di],ax 
    ret         
L$27:
    mov         di,word ptr -36H[bp] 
    dec         di 
    xor         bx,bx 
    mov         bl,byte ptr -58H[bp+di] 
    inc         bx 
    mov         ax,100H 
    xor         dx,dx 
    div         bx 
    mov         word ptr -38H[bp],ax 
    lea         di,-32H[bp] 
    lea         si,-4eH[bp] 
    call        near ptr L$11 
    mov         ax,word ptr -38H[bp] 
    lea         di,-1eH[bp] 
    lea         si,-58H[bp] 
    call        near ptr L$11 
    lea         bx,-0aH[bp] 
    call        near ptr L$1 
    mov         cx,word ptr -34H[bp] 
    sub         cx,word ptr -36H[bp] 
L$28:
    test        cx,cx 
    jl          L$30 
    mov         word ptr -3cH[bp],cx 
    call        near ptr L$17 
    mov         word ptr -3aH[bp],ax 
    lea         si,-1eH[bp] 
    lea         di,-28H[bp] 
    call        near ptr L$11 
    call        near ptr L$19 
    jbe         L$29 
    dec         word ptr -3aH[bp] 
    lea         si,-1eH[bp] 
    lea         di,-28H[bp] 
    call        near ptr L$26 
L$29:
    mov         di,word ptr -3cH[bp] 
    mov         ax,word ptr -3aH[bp] 
    mov         byte ptr -0aH[bp+di],al 
    call        near ptr L$23 
    mov         cx,word ptr -3cH[bp] 
    dec         cx 
    jmp         L$28 
L$30:
    test        byte ptr -42H[bp],1 
    je          L$31 
    mov         ax,word ptr -38H[bp] 
    lea         di,-14H[bp] 
    lea         si,-32H[bp] 
    call        near ptr L$14 
L$31:
    ret         
L$32:
    xor         cx,cx 
    mov         ax,cx 
    sub         ax,word ptr ss:[bx] 
    mov         word ptr ss:[bx],ax 
    mov         ax,cx 
    sbb         ax,word ptr ss:2[bx] 
    mov         word ptr ss:2[bx],ax 
    mov         ax,cx 
    sbb         ax,word ptr ss:4[bx] 
    mov         word ptr ss:4[bx],ax 
    mov         ax,cx 
    sbb         ax,word ptr ss:6[bx] 
    mov         word ptr ss:6[bx],ax 
    ret         
L$33:
    mov         ax,word ptr -48H[bp] 
    test        ax,ax 
    jl          L$34 
    mov         ax,word ptr -52H[bp] 
    test        ax,ax 
    jge         L$36 
    lea         bx,-58H[bp] 
    call        near ptr L$32 
    call        near ptr L$40 
    and         word ptr -42H[bp],2 
    jmp         L$37 
L$34:
    mov         ax,word ptr -52H[bp] 
    test        ax,ax 
    jl          L$35 
    lea         bx,-4eH[bp] 
    call        near ptr L$32 
    call        near ptr L$40 
    jmp         L$37 
L$35:
    lea         bx,-4eH[bp] 
    call        near ptr L$32 
    lea         bx,-58H[bp] 
    call        near ptr L$32 
    call        near ptr L$40 
    and         word ptr -42H[bp],1 
    jmp         L$37 
L$36:
    call        near ptr L$40 
    jmp         L$39 
L$37:
    test        word ptr -42H[bp],2 
    je          L$38 
    lea         bx,-0aH[bp] 
    call        near ptr L$32 
L$38:
    test        word ptr -42H[bp],1 
    je          L$39 
    lea         bx,-14H[bp] 
    call        near ptr L$32 
L$39:
    ret         
L$40:
    push        si 
    push        di 
    lea         bx,-58H[bp] 
    call        near ptr L$3 
    cmp         al,1 
    jne         L$41 
    mov         ax,word ptr -58H[bp] 
    lea         di,-0aH[bp] 
    lea         si,-4eH[bp] 
    call        near ptr L$14 
    test        byte ptr -42H[bp],1 
    je          L$43 
    lea         bx,-14H[bp] 
    call        near ptr L$1 
    mov         word ptr -14H[bp],dx 
    jmp         L$43 
L$41:
    mov         cx,ax 
    lea         bx,-4eH[bp] 
    call        near ptr L$3 
    cmp         al,cl 
    jge         L$42 
    lea         bx,-0aH[bp] 
    call        near ptr L$1 
    test        byte ptr -42H[bp],1 
    je          L$43 
    lea         si,-4eH[bp] 
    lea         di,-14H[bp] 
    call        near ptr L$2 
    jmp         L$43 
L$42:
    xor         ch,ch 
    mov         word ptr -36H[bp],cx 
    xor         ah,ah 
    mov         word ptr -34H[bp],ax 
    call        near ptr L$27 
L$43:
    pop         di 
    pop         si 
    ret         
L$44:
    xchg        word ptr -46H[bp],ax 
    mov         word ptr -44H[bp],ax 
    xor         ax,ax 
    xchg        word ptr -46H[bp],ax 
    push        ax 
    push        bx 
    push        cx 
    push        dx 
    push        word ptr -46H[bp] 
    push        word ptr ss:6[si] 
    push        word ptr ss:4[si] 
    push        word ptr ss:2[si] 
    push        word ptr ss:[si] 
    push        word ptr -44H[bp] 
    ret         
L$45:
    xchg        word ptr -46H[bp],ax 
    mov         word ptr -44H[bp],ax 
    xor         ax,ax 
    xchg        word ptr -46H[bp],ax 
    push        ax 
    push        bx 
    push        cx 
    push        dx 
    push        word ptr -46H[bp] 
    push        word ptr es:6[si] 
    push        word ptr es:4[si] 
    push        word ptr es:2[si] 
    push        word ptr es:[si] 
    push        word ptr -44H[bp] 
    ret         
__U8DQ:
    push        bp 
    mov         bp,sp 
    lea         sp,-44H[bp] 
    call        near ptr L$44 
    mov         word ptr -42H[bp],2 
    call        near ptr L$40 
    lea         sp,-0aH[bp] 
    jmp         L$46 
__U8DR:
    push        bp 
    mov         bp,sp 
    lea         sp,-44H[bp] 
    call        near ptr L$44 
    mov         word ptr -42H[bp],1 
    call        near ptr L$40 
    lea         sp,-14H[bp] 
    jmp         L$46 
__I8DQ:
    push        bp 
    mov         bp,sp 
    lea         sp,-44H[bp] 
    call        near ptr L$44 
    mov         word ptr -42H[bp],2 
    call        near ptr L$33 
    lea         sp,-0aH[bp] 
L$46:
    pop         dx 
    pop         cx 
    pop         bx 
    pop         ax 
    mov         sp,bp 
    pop         bp 
    retf        
__I8DR:
    push        bp 
    mov         bp,sp 
    lea         sp,-44H[bp] 
    call        near ptr L$44 
    mov         word ptr -42H[bp],1 
    call        near ptr L$33 
    lea         sp,-14H[bp] 
    jmp         L$46 
__U8DQE:
    push        bp 
    mov         bp,sp 
    lea         sp,-44H[bp] 
    call        near ptr L$45 
    mov         word ptr -42H[bp],2 
    call        near ptr L$40 
    lea         sp,-0aH[bp] 
    jmp         L$46 
__U8DRE:
    push        bp 
    mov         bp,sp 
    lea         sp,-44H[bp] 
    call        near ptr L$45 
    mov         word ptr -42H[bp],1 
    call        near ptr L$40 
    lea         sp,-14H[bp] 
    jmp         L$46 
__I8DQE:
    push        bp 
    mov         bp,sp 
    lea         sp,-44H[bp] 
    call        near ptr L$45 
    mov         word ptr -42H[bp],2 
    call        near ptr L$33 
    lea         sp,-0aH[bp] 
    jmp         L$46 
__I8DRE:
    push        bp 
    mov         bp,sp 
    lea         sp,-44H[bp] 
    call        near ptr L$45 
    mov         word ptr -42H[bp],1 
    call        near ptr L$33 
    lea         sp,-14H[bp] 
    jmp         L$46 
I8D086_TEXT		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
		END

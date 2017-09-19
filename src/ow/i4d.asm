.387
		PUBLIC	__I4D
		PUBLIC	__U4D
DGROUP		GROUP	_DATA
I4D_TEXT		SEGMENT	WORD PUBLIC USE16 'CODE'
		ASSUME CS:I4D_TEXT, DS:DGROUP, SS:DGROUP
__I4D:
    or          dx,dx 
    js          L$1 
    or          cx,cx 
    jns         __U4D 
    neg         cx 
    neg         bx 
    sbb         cx,0 
    call        far ptr __U4D 
    neg         dx 
    neg         ax 
    sbb         dx,0 
    retf        
L$1:
    neg         dx 
    neg         ax 
    sbb         dx,0 
    or          cx,cx 
    jns         L$2 
    neg         cx 
    neg         bx 
    sbb         cx,0 
    call        far ptr __U4D 
    neg         cx 
    neg         bx 
    sbb         cx,0 
    retf        
L$2:
    call        far ptr __U4D 
    neg         cx 
    neg         bx 
    sbb         cx,0 
    neg         dx 
    neg         ax 
    sbb         dx,0 
    retf        
__U4D:
    or          cx,cx 
    jne         L$5 
    dec         bx 
    je          L$4 
    inc         bx 
    cmp         bx,dx 
    ja          L$3 
    mov         cx,ax 
    mov         ax,dx 
    sub         dx,dx 
    div         bx 
    xchg        ax,cx 
L$3:
    div         bx 
    mov         bx,dx 
    mov         dx,cx 
    sub         cx,cx 
L$4:
    retf        
L$5:
    cmp         cx,dx 
    jb          L$7 
    jne         L$6 
    cmp         bx,ax 
    ja          L$6 
    sub         ax,bx 
    mov         bx,ax 
    sub         cx,cx 
    sub         dx,dx 
    mov         ax,1 
    retf        
L$6:
    sub         cx,cx 
    sub         bx,bx 
    xchg        ax,bx 
    xchg        dx,cx 
    retf        
L$7:
    push        bp 
    push        si 
    sub         si,si 
    mov         bp,si 
L$8:
    add         bx,bx 
    adc         cx,cx 
    jb          L$11 
    inc         bp 
    cmp         cx,dx 
    jb          L$8 
    ja          L$9 
    cmp         bx,ax 
    jbe         L$8 
L$9:
    clc         
L$10:
    adc         si,si 
    dec         bp 
    js          L$14 
L$11:
    rcr         cx,1 
    rcr         bx,1 
    sub         ax,bx 
    sbb         dx,cx 
    cmc         
    jb          L$10 
L$12:
    add         si,si 
    dec         bp 
    js          L$13 
    shr         cx,1 
    rcr         bx,1 
    add         ax,bx 
    adc         dx,cx 
    jae         L$12 
    jmp         L$10 
L$13:
    add         ax,bx 
    adc         dx,cx 
L$14:
    mov         bx,ax 
    mov         cx,dx 
    mov         ax,si 
    xor         dx,dx 
    pop         si 
    pop         bp 
    retf        
I4D_TEXT		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
		END

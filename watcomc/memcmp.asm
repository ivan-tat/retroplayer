.387
		PUBLIC	memcmp_
DGROUP		GROUP	_DATA
MEMCMP_TEXT		SEGMENT	BYTE PUBLIC USE16 'CODE'
		ASSUME CS:MEMCMP_TEXT, DS:DGROUP, SS:DGROUP
memcmp_:
    push        si 
    push        di 
    push        bp 
    push        ds	; added manually
    mov         bp,sp 
    mov         si,cx 
    mov         cx,word ptr 0cH[bp]	; was 0aH[bp]]
    mov         di,bx 
    mov         es,si 
    mov         ds,dx 
    mov         si,ax 
    or          cx,cx 
    repe cmpsb  
    je          L$1 
    sbb         cx,cx 
    sbb         cx,0ffffH 
L$1:
    mov         ax,cx 
    pop         ds	; added manually
    pop         bp 
    pop         di 
    pop         si 
    retf        2 
MEMCMP_TEXT		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
		END

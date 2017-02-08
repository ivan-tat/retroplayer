.387
		PUBLIC	memcpy_
DGROUP		GROUP	_DATA
MEMCPY_TEXT		SEGMENT	BYTE PUBLIC USE16 'CODE'
		ASSUME CS:MEMCPY_TEXT, DS:DGROUP, SS:DGROUP
memcpy_:
    push        si
    push        di
    push        bp
    mov         bp,sp
    mov         di,cx
    mov         cx,word ptr 0aH[bp]
    mov         si,bx
    mov         ds,di
    mov         es,dx
    mov         di,ax
    push        di
    shr         cx,1
    rep movsw
    adc         cx,cx
    rep movsb
    pop         di
    pop         bp
    pop         di
    pop         si
    retf        2
MEMCPY_TEXT		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
		END

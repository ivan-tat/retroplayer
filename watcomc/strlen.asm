.387
		PUBLIC	strlen_
DGROUP		GROUP	_DATA
STRLEN_TEXT		SEGMENT	BYTE PUBLIC USE16 'CODE'
		ASSUME CS:STRLEN_TEXT, DS:DGROUP, SS:DGROUP
strlen_:
    push        cx
    push        di
    mov         di,ax
    mov         es,dx
    mov         cx,0ffffH
    xor         ax,ax
    repne scasb
    not         cx
    dec         cx
    mov         ax,cx
    pop         di
    pop         cx
    retf
STRLEN_TEXT		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
		END

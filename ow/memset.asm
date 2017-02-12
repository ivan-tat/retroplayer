.387
		PUBLIC	memset_
DGROUP		GROUP	_DATA
MEMSET_TEXT		SEGMENT	BYTE PUBLIC USE16 'CODE'
		ASSUME CS:MEMSET_TEXT, DS:DGROUP, SS:DGROUP
memset_:
    push        di 
    mov         di,ax 
    mov         al,bl 
    mov         es,dx 
    push        di 
    mov         ah,al 
    shr         cx,1 
    rep stosw   
    adc         cx,cx 
    rep stosb   
    pop         di 
    mov         ax,di 
    pop         di 
    retf        
MEMSET_TEXT		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
		END

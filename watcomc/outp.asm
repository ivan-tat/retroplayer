.387
		PUBLIC	outp_
DGROUP		GROUP	_DATA
OUTP_TEXT		SEGMENT	BYTE PUBLIC USE16 'CODE'
		ASSUME CS:OUTP_TEXT, DS:DGROUP, SS:DGROUP
outp_:
    push        bx 
    mov         bx,ax 
    mov         al,dl 
    mov         dx,bx 
    out         dx,al 
    pop         bx 
    retf        
OUTP_TEXT		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
		END

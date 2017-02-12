.387
		PUBLIC	inp_
DGROUP		GROUP	_DATA
INP_TEXT		SEGMENT	BYTE PUBLIC USE16 'CODE'
		ASSUME CS:INP_TEXT, DS:DGROUP, SS:DGROUP
inp_:
    push        dx 
    mov         dx,ax 
    in          al,dx 
    sub         ah,ah 
    pop         dx 
    retf        
INP_TEXT		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
		END

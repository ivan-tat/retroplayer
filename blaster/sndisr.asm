.387
.386p
		PUBLIC	GETSOUNDHWISR
		PUBLIC	SETSOUNDHWISRCALLBACK
		EXTRN	SOUNDHWISR:BYTE
		EXTRN	SOUNDHWISRCALLBACK:BYTE
DGROUP		GROUP	_DATA
SNDISR_TEXT		SEGMENT	BYTE PUBLIC USE16 'CODE'
		ASSUME CS:SNDISR_TEXT, DS:DGROUP, SS:DGROUP
GETSOUNDHWISR:
    mov         ax,offset SOUNDHWISR 
    mov         dx,seg SOUNDHWISR 
    retf        
SETSOUNDHWISRCALLBACK:
    push        bp 
    mov         bp,sp 
    mov         ax,word ptr 6[bp] 
    mov         word ptr SOUNDHWISRCALLBACK,ax 
    mov         ax,word ptr 8[bp] 
    mov         word ptr SOUNDHWISRCALLBACK+2,ax 
    pop         bp 
    retf        4 
SNDISR_TEXT		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
		END

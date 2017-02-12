.387
		PUBLIC	intr_
		EXTRN	_DoINTR_:BYTE
DGROUP		GROUP	_DATA
INTR_TEXT		SEGMENT	BYTE PUBLIC USE16 'CODE'
		ASSUME CS:INTR_TEXT, DS:DGROUP, SS:DGROUP
intr_:
    push        dx 
    push        si 
    push        di 
    mov         si,ax 
    mov         dx,cx 
    mov         ax,bx 
    mov         bx,si 
    call        far ptr _DoINTR_ 
    pop         di 
    pop         si 
    pop         dx 
    retf        
INTR_TEXT		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
		END

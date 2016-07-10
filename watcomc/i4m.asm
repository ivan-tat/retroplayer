.387
		PUBLIC	__U4M
		PUBLIC	__I4M
DGROUP		GROUP	_DATA
I4D_TEXT		SEGMENT	WORD PUBLIC USE16 'CODE'
		ASSUME CS:I4D_TEXT, DS:DGROUP, SS:DGROUP
__U4M:
__I4M:
    xchg        ax,bx 
    push        ax 
    xchg        ax,dx 
    or          ax,ax 
    je          L$1 
    mul         dx 
L$1:
    xchg        ax,cx 
    or          ax,ax 
    je          L$2 
    mul         bx 
    add         cx,ax 
L$2:
    pop         ax 
    mul         bx 
    add         dx,cx 
    retf        
I4D_TEXT		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
		END

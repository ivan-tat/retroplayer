.387
		PUBLIC	_DoINTR_
DGROUP		GROUP	_DATA
DOINTR_TEXT		SEGMENT	WORD PUBLIC USE16 'CODE'
		ASSUME CS:DOINTR_TEXT, DS:DGROUP, SS:DGROUP
_DoINTR_:
    push        bp 
    push        ds 
    push        dx 
    push        ax 
    xor         bh,bh 
    mov         cx,bx 
    shl         bx,1 
    add         bx,cx 
    add         bx,offset inttable 
    mov         cx,cs 
    mov         es,cx 
    call        near ptr doit 
    push        bx 
    push        ds 
    push        bp 
    mov         bp,sp 
    lds         bx,dword ptr 6[bp] 
    pushf       
    pop         word ptr 12H[bx] 
    mov         word ptr [bx],ax 
    mov         word ptr 4[bx],cx 
    mov         word ptr 6[bx],dx 
    mov         word ptr 0aH[bx],si 
    mov         word ptr 0cH[bx],di 
    pop         word ptr 8[bx] 
    pop         word ptr 0eH[bx] 
    pop         word ptr 2[bx] 
    mov         word ptr 10H[bx],es 
    add         sp,4 
    pop         ds 
    pop         bp 
    retf        
doit:
    push        es 
    push        bx 
    mov         ds,dx 
    mov         bx,ax 
    push        word ptr 0eH[bx] 
    mov         ax,word ptr [bx] 
    mov         cx,word ptr 4[bx] 
    mov         dx,word ptr 6[bx] 
    mov         si,word ptr 0aH[bx] 
    mov         di,word ptr 0cH[bx] 
    mov         bp,word ptr 8[bx] 
    mov         es,word ptr 10H[bx] 
    mov         bx,word ptr 2[bx] 
    pop         ds 
    retf        
inttable:
    int         0 
    ret         
    int         1 
    ret         
    int         2 
    ret         
    int         3 
    nop         
    ret         
    int         4 
    ret         
    int         5 
    ret         
    int         6 
    ret         
    int         7 
    ret         
    int         8 
    ret         
    int         9 
    ret         
    int         0aH 
    ret         
    int         0bH 
    ret         
    int         0cH 
    ret         
    int         0dH 
    ret         
    int         0eH 
    ret         
    int         0fH 
    ret         
    int         10H 
    ret         
    int         11H 
    ret         
    int         12H 
    ret         
    int         13H 
    ret         
    int         14H 
    ret         
    int         15H 
    ret         
    int         16H 
    ret         
    int         17H 
    ret         
    int         18H 
    ret         
    int         19H 
    ret         
    int         1aH 
    ret         
    int         1bH 
    ret         
    int         1cH 
    ret         
    int         1dH 
    ret         
    int         1eH 
    ret         
    int         1fH 
    ret         
    int         20H 
    ret         
    int         21H 
    ret         
    int         22H 
    ret         
    int         23H 
    ret         
    int         24H 
    ret         
    jmp         near ptr L$1 
    jmp         near ptr L$3 
    int         27H 
    ret         
    int         28H 
    ret         
    int         29H 
    ret         
    int         2aH 
    ret         
    int         2bH 
    ret         
    int         2cH 
    ret         
    int         2dH 
    ret         
    int         2eH 
    ret         
    int         2fH 
    ret         
    int         30H 
    ret         
    int         31H 
    ret         
    int         32H 
    ret         
    int         33H 
    ret
    int         34H
    ret
    int         35H
    ret
    int         36H
    ret
    int         37H
    ret
    int         38H
    ret
    int         39H
    ret
    int         3aH
    ret
    int         3bH
    ret
    int         3cH
    ret
    int         3dH
    ret         
    int         3eH 
    ret         
    int         3fH 
    ret         
    int         40H 
    ret         
    int         41H 
    ret         
    int         42H 
    ret         
    int         43H 
    ret         
    int         44H 
    ret         
    int         45H 
    ret         
    int         46H 
    ret         
    int         47H 
    ret         
    int         48H 
    ret         
    int         49H 
    ret         
    int         4aH 
    ret         
    int         4bH 
    ret         
    int         4cH 
    ret         
    int         4dH 
    ret         
    int         4eH 
    ret         
    int         4fH 
    ret         
    int         50H 
    ret         
    int         51H 
    ret         
    int         52H 
    ret         
    int         53H 
    ret         
    int         54H 
    ret         
    int         55H 
    ret         
    int         56H 
    ret         
    int         57H 
    ret         
    int         58H 
    ret         
    int         59H 
    ret         
    int         5aH 
    ret         
    int         5bH 
    ret         
    int         5cH 
    ret         
    int         5dH 
    ret         
    int         5eH 
    ret         
    int         5fH 
    ret         
    int         60H 
    ret         
    int         61H 
    ret         
    int         62H 
    ret         
    int         63H 
    ret         
    int         64H 
    ret         
    int         65H 
    ret         
    int         66H 
    ret         
    int         67H 
    ret         
    int         68H 
    ret         
    int         69H 
    ret         
    int         6aH 
    ret         
    int         6bH 
    ret         
    int         6cH 
    ret         
    int         6dH 
    ret         
    int         6eH 
    ret         
    int         6fH 
    ret         
    int         70H 
    ret         
    int         71H 
    ret         
    int         72H 
    ret         
    int         73H 
    ret         
    int         74H 
    ret         
    int         75H 
    ret         
    int         76H 
    ret         
    int         77H 
    ret         
    int         78H 
    ret         
    int         79H 
    ret         
    int         7aH 
    ret         
    int         7bH 
    ret         
    int         7cH 
    ret         
    int         7dH 
    ret         
    int         7eH 
    ret         
    int         7fH 
    ret         
    int         80H 
    ret         
    int         81H 
    ret         
    int         82H 
    ret         
    int         83H 
    ret         
    int         84H 
    ret         
    int         85H 
    ret         
    int         86H 
    ret         
    int         87H 
    ret         
    int         88H 
    ret         
    int         89H 
    ret         
    int         8aH 
    ret         
    int         8bH 
    ret         
    int         8cH 
    ret         
    int         8dH 
    ret         
    int         8eH 
    ret         
    int         8fH 
    ret         
    int         90H 
    ret         
    int         91H 
    ret         
    int         92H 
    ret         
    int         93H 
    ret         
    int         94H 
    ret         
    int         95H 
    ret         
    int         96H 
    ret         
    int         97H 
    ret         
    int         98H 
    ret         
    int         99H 
    ret         
    int         9aH 
    ret         
    int         9bH 
    ret         
    int         9cH 
    ret         
    int         9dH 
    ret         
    int         9eH 
    ret         
    int         9fH 
    ret         
    int         0a0H 
    ret         
    int         0a1H 
    ret         
    int         0a2H 
    ret         
    int         0a3H 
    ret         
    int         0a4H 
    ret         
    int         0a5H 
    ret         
    int         0a6H 
    ret         
    int         0a7H 
    ret         
    int         0a8H 
    ret         
    int         0a9H 
    ret         
    int         0aaH 
    ret         
    int         0abH 
    ret         
    int         0acH 
    ret         
    int         0adH 
    ret         
    int         0aeH 
    ret         
    int         0afH 
    ret         
    int         0b0H 
    ret         
    int         0b1H 
    ret         
    int         0b2H 
    ret         
    int         0b3H 
    ret         
    int         0b4H 
    ret         
    int         0b5H 
    ret         
    int         0b6H 
    ret         
    int         0b7H 
    ret         
    int         0b8H 
    ret         
    int         0b9H 
    ret         
    int         0baH 
    ret         
    int         0bbH 
    ret         
    int         0bcH 
    ret         
    int         0bdH 
    ret         
    int         0beH 
    ret         
    int         0bfH 
    ret         
    int         0c0H 
    ret         
    int         0c1H 
    ret         
    int         0c2H 
    ret         
    int         0c3H 
    ret         
    int         0c4H 
    ret         
    int         0c5H 
    ret         
    int         0c6H 
    ret         
    int         0c7H 
    ret         
    int         0c8H 
    ret         
    int         0c9H 
    ret         
    int         0caH 
    ret         
    int         0cbH 
    ret         
    int         0ccH 
    ret         
    int         0cdH 
    ret         
    int         0ceH 
    ret         
    int         0cfH 
    ret         
    int         0d0H 
    ret         
    int         0d1H 
    ret         
    int         0d2H 
    ret         
    int         0d3H 
    ret         
    int         0d4H 
    ret         
    int         0d5H 
    ret         
    int         0d6H 
    ret         
    int         0d7H 
    ret         
    int         0d8H 
    ret         
    int         0d9H 
    ret         
    int         0daH 
    ret         
    int         0dbH 
    ret         
    int         0dcH 
    ret         
    int         0ddH 
    ret         
    int         0deH 
    ret         
    int         0dfH 
    ret         
    int         0e0H 
    ret         
    int         0e1H 
    ret         
    int         0e2H 
    ret         
    int         0e3H 
    ret         
    int         0e4H 
    ret         
    int         0e5H 
    ret         
    int         0e6H 
    ret         
    int         0e7H 
    ret         
    int         0e8H 
    ret         
    int         0e9H 
    ret         
    int         0eaH 
    ret         
    int         0ebH 
    ret         
    int         0ecH 
    ret         
    int         0edH 
    ret         
    int         0eeH 
    ret         
    int         0efH 
    ret         
    int         0f0H 
    ret         
    int         0f1H 
    ret         
    int         0f2H 
    ret         
    int         0f3H 
    ret         
    int         0f4H 
    ret         
    int         0f5H 
    ret         
    int         0f6H 
    ret         
    int         0f7H 
    ret         
    int         0f8H 
    ret         
    int         0f9H 
    ret         
    int         0faH 
    ret         
    int         0fbH 
    ret         
    int         0fcH 
    ret         
    int         0fdH 
    ret         
    int         0feH 
    ret         
    int         0ffH 
    ret         
L$1:
    int         25H 
    jae         L$2 
    popf        
    stc         
    ret         
L$2:
    popf        
    clc         
    ret         
L$3:
    int         26H 
    jae         L$4 
    popf        
    stc         
    ret         
L$4:
    popf        
    clc         
    ret         
DOINTR_TEXT		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA		ENDS
		END

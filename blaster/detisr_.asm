; isrdet.asm - interrupt service routines for automatic detection of
; hardware interrupts.
;
; This is free and unencumbered software released into the public domain.

.model large,pascal
.386p
.387

public GETDETISR
extern DETISRCALLBACK:dword

deflabel macro name, index
&name&index:
endm

defoff macro name, index
dw offset &name&index
endm

DGROUP group _DATA

DETISR__TEXT segment word public use16 'CODE'
assume cs:DETISR__TEXT, ds:DGROUP, ss:DGROUP

index=0
repeat 16
align 2
deflabel DETISR_, %index
	push	ax
	mov	al,index
	jmp	short DETISR
index=index+1
endm

align 2
DETISR:
	push	cx
	push	dx
	push	bx
	push	bp
	push	si
	push	di
	push	ds
	push	es
	push	fs
	push	gs

	xor	ah,ah
	push	ax
	mov	ax,DGROUP
	mov	ds,ax
	mov	ax,word ptr [DETISRCALLBACK]
	or	ax,word ptr [DETISRCALLBACK+2]
	jz	near ptr DETISR$skip
	call	dword ptr DETISRCALLBACK
DETISR$skip:

	pop	gs
	pop	fs
	pop	es
	pop	ds
	pop	di
	pop	si
	pop	bp
	pop	bx
	pop	dx
	pop	cx
	pop	ax
	iret

GETDETISR proc far num:byte
	movzx	bx,byte ptr [num]
	shl	bx,1
	mov	ax,word ptr [DETISRTAB][bx]
	mov	dx,cs
	ret
GETDETISR endp

DETISR__TEXT ends

_DATA segment word public use16 'DATA'
assume cs:_DATA

index=0
DETISRTAB:
repeat 16
	defoff DETISR_, %index
index=index+1
endm

_DATA ends

end

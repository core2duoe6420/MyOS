[bits 32]

global	keyboard_intr
extern	do_keyboard_intr

section .text

keyboard_intr:
	pushad
	push	ds
	push	es
	push	gs
	push	fs
	mov	eax,0x10
	mov	ds,ax
	mov	es,ax
	mov	gs,ax
	mov	fs,ax

	mov	al,0
	in	al,0x60

	push	eax
	call	do_keyboard_intr
	add	esp,4

	in	al,0x61
DW 0x00eb,0x00eb
	or	al,0x80
DW 0x00eb,0x00eb
	out	0x61,al
DW 0x00eb,0x00eb
	and	al,0x7f
	out	0x61,al
	mov	al,0x20
	out	0x20,al


	pop	fs
	pop	gs
	pop	es
	pop	ds
	popad
	iret

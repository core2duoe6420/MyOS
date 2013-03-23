start:
	jmp	0x07C0:go
go:	mov	ax,cs
	mov	ds,ax
	mov	ss,ax
	mov	sp,0x400
load_system:
	;head 0 track 0 sector 2
	mov	dx,0
	mov	cx,2
	mov	ax,0x1000
	mov	es,ax
	xor	bx,bx
	mov	ax,0x217
	int	13h
	jc	die
	;head 1 track 0
	mov	dx,0x100
	mov	cx,1
	mov	ax,0x1220
	mov	es,ax
	xor	bx,bx
	mov	ax,0x218
	int	13h
	jc	die
	;head 0 track 1
	mov	dx,0
	mov	cx,0x101
	mov	ax,0x1220+0x240
	mov	es,ax
	xor	bx,bx
	mov	ax,0x218
	int	13h
	jc	die
	;head 1 track 1
	mov	dx,0x100
	mov	cx,0x101
	mov	ax,0x1220+0x240*2
	mov	es,ax
	xor	bx,bx
	mov	ax,0x218
	int	13h
	jc	die
	;head 0 track 2
	mov	dx,0
	mov	cx,0x201
	mov	ax,0x1220+0x240*3
	mov	es,ax
	xor	bx,bx
	mov	ax,0x218
	int	13h
	jc	die
	;head 1 track 2
	mov	dx,0x100
	mov	cx,0x201
	mov	ax,0x1220+0x240*4
	mov	es,ax
	xor	bx,bx
	mov	ax,0x218
	int	13h
	jc	die

	jmp	ok_load
die:	jmp	die
ok_load:
	;open A20
	call	empty_8042
	mov	al,0xD1
	out	0x64,al
	call	empty_8042
	mov	al,0xDF
	out	0x60,al
	call	empty_8042

	stc
	xor	cx,cx
	xor	dx,dx

	mov	ax,0xe801
	int	0x15
	jc	die
	cmp	cx,0
	jne	e801usecxdx
	cmp	dx,0
	jne	e801usecxdx
	mov	cx,ax
	mov	dx,bx

e801usecxdx:
	and	edx,0xffff
	shl	edx,6
	mov	DWORD [0],edx
	and	ecx,0xffff
	add	DWORD [0],ecx

	cli
	lidt	[idt_48]
	lgdt	[gdt_48]
	mov	eax,cr0
	or	eax,1
	mov	cr0,eax
	jmp	8:DWORD 0x10000

empty_8042:
	nop
	nop
	in	al,0x64
	test	al,2
	jnz	empty_8042
	ret
ALIGN 16
idt_48:	DW	0x7FF
	DD	0x20096000
gdt_48:	DW	0x7FF,0x7C00+gdt,0

TIMES 470-($-$$) DB 0	
gdt:
	DW 	0,0,0,0
	DW	0xFFFF,0x0000,0x9A00,0x00CF
	DW	0xFFFF,0x0000,0x9300,0x00CF
	DW	0xFFFF,0x0000,0xFA00,0x00CF
	DW	0xFFFF,0x0000,0xF300,0x00CF

TIMES 510-($-$$) DB 0
	DW	0xAA55

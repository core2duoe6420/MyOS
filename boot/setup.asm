[bits 32]
PAGE_OFFSET EQU 0x20000000

extern main

global START,new_gdt,new_idt

section .text
START:
	mov	eax,0x10
	mov	ds,ax
	mov	ss,ax
	mov	fs,ax
	mov	gs,ax
	mov	es,ax
	;copy kernel [64K-600K] to 1MB
	mov	ecx,0x96000-0x10000
	mov	esi,0x10000
	mov	edi,0x100000
	rep	movsb
	mov	esp,0x96000 ;600KB
	;copy gdt to 602KB
	;but not reload gdt now
	;we will reload gdt after switch on paging
	;gdt at 0x7c00 should be safe now
	mov	ecx,40
	mov	esi,0x7DD6
	mov	edi,0x96800
	rep	movsb

STEP_TO_MAIN:
	push	main-PAGE_OFFSET
	ret

new_gdt:DW	0x7FF
	DD	0x20096800
new_idt:DW	0x7FF
	DD	0x20096000

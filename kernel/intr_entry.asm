[bits 32]

extern do_signal

;sys_calls
extern fork
extern display
extern exit
extern gets
extern pause

;interrupt
extern do_timer_intr
extern do_general_intr

;traps
extern do_divide_error
extern do_debug
extern do_nmi
extern do_int3
extern do_overflow
extern do_bounds
extern do_invalid_op
extern do_device_not_available
extern do_double_fault
extern do_coprocessor_segment_overrun
extern do_invalid_TSS
extern do_segment_not_present
extern do_stack_segment
extern do_general_protection
extern do_page_fault
extern do_reserved
extern do_coprocessor_error
extern do_alignment_check

;traps
global divide_error
global debug
global nmi
global int3
global overflow
global bounds
global invalid_op
global device_not_available
global double_fault
global coprocessor_segment_overrun
global invalid_TSS
global segment_not_present
global stack_segment
global general_protection
global page_fault
global reserved
global coprocessor_error
global alignment_check

;system call & interrupt
global timer_intr
global general_intr

global sys_call

sys_call_table DD	fork,exit,gets,display,pause

trap:
	xchg	eax,[esp+4]
	xchg	ebx,[esp]
	push	ecx
	push	edx
	push	esi
	push	edi
	push	ebp
	push	ds
	push	es
	push	gs
	push	fs
	mov	edx,0x10
	mov	ds,dx
	mov	es,dx
	mov	gs,dx
	mov	fs,dx

	push	eax
	lea	edx,[esp+4]
	push	edx
	call	ebx
	add	esp,8

	pop	fs
	pop	gs
	pop	es
	pop	ds
	pop	ebp
	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	iret

general_intr:
	push	0
	push	do_general_intr
	jmp	trap

timer_intr:
	pushad
	mov	al,0x20
	out	0x20,al
	call	do_timer_intr
	popad
	iret

;the following are system calls
sys_call:
	push	eax
	push	edi
	push	esi
	push	ebp
	push	ds
	push	es
	push	fs
	push	gs
	push	eax
	mov	eax,0x10
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	pop	eax
	push	edx
	push	ecx
	push	ebx
	call	[sys_call_table+eax*4]
	add	esp,12
	call	do_signal
	pop	gs
	pop	fs
	pop	es
	pop	ds
	pop	ebp
	pop	esi
	pop	edi
	pop	eax
	iret

;the following are traps
;those which needs "push 0"
;doesn't has error code
;I push 0 to unify two ISR
divide_error:
	push	0
	push	do_divide_error
	jmp	trap

debug:
	push	0
	push	do_debug
	jmp	trap

nmi:
	push	0
	push	do_debug
	jmp	trap

int3:
	push	0
	push	do_int3
	jmp	trap

overflow:
	push	0
	push	do_overflow
	jmp	trap

bounds:
	push	0
	push	do_bounds
	jmp	trap

invalid_op:
	push	0
	push	do_invalid_op
	jmp	trap

device_not_available:
	push	0
	push	do_device_not_available
	jmp	trap

coprocessor_segment_overrun:
	push	0
	push	do_coprocessor_segment_overrun
	jmp	trap

reserved:
	push	0
	push	do_reserved
	jmp	trap

coprocessor_error:
	push	eax
	xor	al,al
	out 	0xf0,al
	mov	al,0x20
	out	0x20,al
	out	0xa0,al
	pop	eax
	push	0
	push	do_coprocessor_error
	jmp	trap

double_fault:
	push	do_double_fault
	jmp	trap

invalid_TSS:
	push	do_invalid_TSS
	jmp	trap

segment_not_present:
	push	do_segment_not_present
	jmp	trap

stack_segment:
	push	do_stack_segment
	jmp	trap

alignment_check:
	push	do_alignment_check
	jmp	trap

general_protection:
	push	do_general_protection
	jmp	trap

page_fault:
	push	do_page_fault
	jmp	trap



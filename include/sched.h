#ifndef __SCHED_H
#define __SCHED_H

#include <lib.h>
#include <descriptor.h>
#include <const.h>
#include <page.h>

extern struct task_struct *tasks[TASK_MAX_NR];
extern struct task_struct *current;

#define ldt_init {\
{0,0,0,0,0,0,0,0,0,0,0,0,0},\
{0xffff,0,0,0xc,1,3,1,0xf,0,0,1,1,0},\
{0xffff,0,0,0x2,1,3,1,0xf,0,0,1,1,0}}

#define tss_init {\
	0,0,0x10,0,0,0,0,PDE_START,\
	0,0,0,0,0,0,\
	0,0,0,0,\
	0,0,0,0,0,0,\
	0x18,0,0,0x8000}

#define task0_pagelist_init {\
{0,0},{0,0},{0,0},{0,0},{0,0}}

#define task0_init {\
	0,0,0,\
	0,0,\
	0,0,0,0,0,0,0,NULL,NULL,NULL,NULL,0,0,0,\
	ttys[0],task0_pagelist_init,tss_init}

struct tss_struct{
	uint32 backlink;/*high 16bits reserved*/
	uint32 esp0;
	uint32 ss0;	/*high 16bits reserved*/
	uint32 esp1;
	uint32 ss1;/*high 16bits reserved*/
	uint32 esp2;
	uint32 ss2;/*high 16bits reserved*/
	uint32 cr3;
	uint32 eip;
	uint32 eflags;
	uint32 eax,ecx,edx,ebx;
	uint32 esp,ebp,esi,edi;
	uint32 es,cs,ss,ds,fs,gs;/*high 16bits reserved*/
	uint32 ldt;/*high 16bits reserved*/
	uint32 t_mark:1;
	uint32 reserved:15;
	uint32 iomap:16;
};

struct task_struct{
	uint32 state; /*TASK_XXX*/
	uint32 counter; /* time slice*/
	uint32 priority;

	uint32 signal;
	uint32 sig_block;

	int exit_code;
	uint32 start_code;
	uint32 end_code;
	uint32 end_data;
	uint32 brk;/*length of code+data*/
	uint32 start_stack;
	uint32 pid;

	/* we don't have task group and session either*/

	struct task_struct *father_ptr;
	struct task_struct *latest_child_ptr;/*newest child task of father*/
	struct task_struct *younger_ptr;/*child task created after me*/
	struct task_struct *older_ptr;/*child task created before me*/
	uint32 usertime;
	uint32 systime;/*jiffies*/
	uint32 start_time;

	struct tty_struct * volatile tty;

	/* we don't have keys on file system now:(*/
	struct mm_struct mm;
	struct tss_struct tss;
};

union task_page{
	struct task_struct task;
	char kernel_stack[PAGE_SIZE];
};

#define move_to_user_mode() \
__asm__ ("movl $0x20096000,%%eax\n\t" \
	"pushl $0x17\n\t" \
	"pushl %%eax\n\t" \
	"pushfl\n\t" \
	"pushl $0x0f\n\t" \
	"pushl $1f\n\t" \
	"iret\n" \
	"1:\tmovl $0x17,%%eax\n\t" \
	"mov %%ax,%%ds\n\t" \
	"mov %%ax,%%es\n\t" \
	"mov %%ax,%%fs\n\t" \
	"mov %%ax,%%gs\n\t" \
	::)

#define load_tr_ldr() \
__asm__("pushl %%eax\n\t" \
	"movl $0x20,%%eax\n\t" \
	"ltr %%ax\n\t" \
	"movl $0x18,%%eax\n\t" \
	"lldt %%ax\n\t" \
	"popl %%eax"::)

#define switch_task(n) \
do{\
	set_tss_desc((uint32)&tasks[n]->tss); \
	__asm__("ljmp $0x20,$0"); \
}while(0)

extern void schedule();
extern void sleep_on(struct task_struct **p);
extern void interruptible_sleep_on(struct task_struct **p);
extern void wake_up(struct task_struct **p);
extern void pause();

#endif

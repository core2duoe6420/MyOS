#include <sched.h>
#include <string.h>
#include <memory.h>
#include <tty.h>
struct task_struct *current;
struct task_struct *tasks[TASK_MAX_NR];

void set_ldt_desc(uint32 address){
	struct desc_struct *desc=LDT_ENTRY_PTR;
	int limit=64;/*ldt limit 64b*/
	desc->limit_low=limit & 0xffff;
	desc->limit_high=limit>>16;
	desc->base_high=limit>>16;
	desc->base_low=address & 0xffff;
	desc->base_mid=(address>>16) & 0xff;
	desc->base_high=address>>24;
	desc->dpl=3;
	desc->s_flag=0;
	desc->present=1;
	desc->avl=0;
	desc->granularity=0;
	desc->db=0;
	desc->reserved=0;
	desc->type=2; /* 0010 ldt*/
}

void set_tss_desc(uint32 address){
	struct desc_struct *desc=TSS_ENTRY_PTR;
	int limit=200;/*tss limit 200b*/
	desc->limit_low=limit & 0xffff;
	desc->limit_high=limit>>16;
	desc->base_high=limit>>16;
	desc->base_low=address & 0xffff;
	desc->base_mid=(address>>16) & 0xff;
	desc->base_high=address>>24;
	desc->dpl=0;
	desc->s_flag=0;
	desc->present=1;
	desc->avl=0;
	desc->granularity=0;
	desc->db=0;
	desc->reserved=0;
	/* 1001 32-bit tss available-busy 0. 
	   I'm not sure how busy bit works.
	   I think it is managed by os*/
	desc->type=0x9; 
}

void initial_ldt(){
	struct desc_struct ldt[]=ldt_init;
	memcpy(LDT_PTR,ldt,sizeof(struct desc_struct)*3);
	set_ldt_desc((uint32)LDT_PTR);
}

void initial_sched(){
	struct page_struct *page;
	struct task_struct tmp=task0_init;
	struct task_struct *init;

	initial_ldt();

	page=alloc_page(0,PAGE_KERNEL|PAGE_TASK_STRUCT);
	init=(struct task_struct*)page_to_address(page);
	memcpy(init,&tmp,sizeof(struct task_struct));
	init->tss.esp0=page_to_address(page)+PAGE_SIZE;
	
	current=init;
	tasks[0]=init;
	init->counter=20;
	for(int i=1;i<TASK_MAX_NR;i++)
		tasks[i]=NULL;

	set_tss_desc((uint32)&tasks[0]->tss);
	/* ltr & lldr */
	load_tr_ldr();
	/* Clear NT. I'm not sure how it works*/
	__asm__("pushfl ; andl $0xffffbfff,(%esp) ; popfl");
}

void schedule(){
	int next=0;
	int repeated=0;
	uint32 max_prio=0;

repeat:
	for(int i=1;i<TASK_MAX_NR;i++){
		if(tasks[i] && (!tasks[i]->state) 
			&& tasks[i]->counter > max_prio){
			max_prio=tasks[i]->counter;
			next=i;
		}
	}

	//reset the counter of all RUNNING tasks
	if((!next) && (!repeated)){
		for(int i=1;i<TASK_MAX_NR;i++){
			if(tasks[i] && (!tasks[i]->state)){
				tasks[i]->counter=tasks[i]->priority;
			}
		}
		repeated=1;
		goto repeat;
	}

	//pick current task?
	if(tasks[next]==current)
		return;
	//task counter unnormally big means preempt
	if(tasks[next]->counter>=150)
		tasks[next]->counter=tasks[next]->priority;
	current=tasks[next];
	switch_task(next);
}

static inline void __sleep_on(struct task_struct **p, int state){
	struct task_struct *tmp;

	if (!p)
		return;
	if (current==tasks[0])
		printk("task[0] trying to sleep");
	tmp=*p;
	*p=current;
	current->state=state;
repeat:	schedule();
	if (*p && *p!=current) {
		(**p).state=0;
		current->state=TASK_UNINTERRUPTIBLE;
		goto repeat;
	}
	if (!*p)
		printk("Warning: *P = NULL\n");
	if (*p=tmp)
		tmp->state=0;
}

void interruptible_sleep_on(struct task_struct **p){
	__sleep_on(p,TASK_INTERRUPTIBLE);
}

void sleep_on(struct task_struct **p){
	__sleep_on(p,TASK_UNINTERRUPTIBLE);
}

void wake_up(struct task_struct **p){
	if (p && *p){
		if((**p).state==TASK_STOPPED)
			printk("wake_up: TASK_STOPPED");
		if((**p).state==TASK_ZOMBIE)
			printk("wake_up: TASK_ZOMBIE");
		(**p).state=0;
	}
}

void pause(){
	current->state = TASK_INTERRUPTIBLE;
	schedule();
}

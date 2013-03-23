#include <memory.h>
#include <page.h>
#include <sched.h>
#include <string.h>
#include <tty.h>

/* it's just a fake fork :) */
int fork(void *entry,int count){
	struct page_struct *codedata,*stack;
	struct page_struct *pde_p,*pte_p;
	struct page_struct *task_page;

	struct pde_struct *pde;
	struct pte_struct *pte;
	struct task_struct *task;
	uint32 task_address;
	
	int idx=TASK_MAX_NR;

	for(idx=1;idx<TASK_MAX_NR;idx++)
		if((!tasks[idx]) || tasks[idx]->state==TASK_ZOMBIE)
			break;

	if(idx==TASK_MAX_NR){
		printk("reach max tasks!\n");
		return -1;
	}

	/* if there is a ZOMBIE task_struct
	 * use it first*/
	if(!tasks[idx])
		task_page=alloc_page(0,PAGE_TASK_STRUCT);
	else
		task_page=address_to_page((uint32)tasks[idx]);
	task_address=page_to_address(task_page);
	task=(struct task_struct*)task_address;

	codedata=alloc_page(0,PAGE_TASK_CODE|PAGE_TASK_DATA);
	stack=alloc_page(0,PAGE_TASK_STACK);
	pde_p=alloc_page(0,PAGE_PDE);
	pte_p=alloc_page(0,PAGE_PTE);

	pde=(struct pde_struct*)page_to_address(pde_p);
	pte=(struct pte_struct*)page_to_address(pte_p);

	/* set tss */
	task->tss.esp=TASK_STACK;
	task->tss.esp0=task_address+PAGE_SIZE;
	task->tss.ss0=0x10;
	task->tss.eip=TASK_ENTRY;
	task->tss.es=0x17;
	task->tss.ss=0x17;
	task->tss.ds=0x17;
	task->tss.fs=0x17;
	task->tss.gs=0x17;
	task->tss.cs=0xf;
	task->tss.ldt=0x18;
	task->tss.cr3=__pa(page_to_address(pde_p));
	/* set if */
	task->tss.eflags |=0x200;

	/* set mm */
	task->mm.code.first=codedata;
	task->mm.code.nr++;
	task->mm.data.first=codedata;
	task->mm.data.nr++;
	task->mm.pde.first=pde_p;
	task->mm.pde.nr++;
	task->mm.pte.first=pte_p;
	task->mm.pte.nr++;
	task->mm.stack.first=stack;
	task->mm.stack.nr++;

	/* set page table */
	/* share the kernel map */
	memcpy(pde,(void*)__va(PDE_START),sizeof(struct pde_struct));
	/* we have to set the va of pte to pde first,
	 * then set pa into pde
 	 * otherwise it can't find pte in kernel linear address*/
	set_page_structure(pde->pte+32,1,1,1,(uint32)pte);
	set_user_pte(pde,32,0,__pa(page_to_address(codedata)));
	set_page_structure(pde->pte+63,1,1,1,(uint32)pte);
	set_user_pte(pde,63,1023,__pa(page_to_address(stack)));
	set_page_structure(pde->pte+32,1,1,1,__pa((uint32)pte));
	set_page_structure(pde->pte+63,1,1,1,__pa((uint32)pte));

	/* copy code. we don't have hard-driver now */
	memcpy((void*)page_to_address(codedata),entry,count);
	/* set task_struct */
	task->older_ptr=current->latest_child_ptr;
	task->younger_ptr=NULL;
	task->latest_child_ptr=NULL;
	task->father_ptr=current;
	current->latest_child_ptr=task;
	
	task->tty=task->father_ptr->tty;

	/* modify pointer of tty_struct */
	task->tty->cur_task=task;
	
	task->signal=0;
	task->sig_block=0;

	task->pid=idx;

	printk("fork pid:%d,fahther:%d\n",idx,current->pid);
	get_free_page_info();

	tasks[idx]=task;
	task->state=TASK_RUNNING;
	task->counter=20;
	task->priority=20;

	return idx;
}

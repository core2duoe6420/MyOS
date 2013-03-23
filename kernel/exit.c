#include <memory.h>
#include <page.h>
#include <sched.h>
#include <io.h>
#include <tty.h>
int exit(int exitcode){
	struct task_struct *task=current;
	
	/* we forbid intr now */
	cli();
	/* since the task is exiting
	 * we switch the pde to kernel pde */
	__asm__("movl	%%eax,%%cr3"::"a"(PDE_START));
	
	free_page(task->mm.code.first);
	/* since we have code & data on one page
	 * we will get a "trying to free unused page" info
	 * when we try to free data page
	 */
	free_page(task->mm.data.first);
	free_page(task->mm.pte.first);
	free_page(task->mm.pde.first);
	free_page(task->mm.stack.first);

	if(task->younger_ptr)
		task->younger_ptr->older_ptr=task->older_ptr;
	if(task->older_ptr)
		task->older_ptr->younger_ptr=task->younger_ptr;
	task->father_ptr->latest_child_ptr=task->older_ptr;
	if(task->latest_child_ptr){
		struct task_struct *child;
		child=task->latest_child_ptr;
		for(;child;child=child->older_ptr)
			child->father_ptr=tasks[0];
	}

	task->tty->cur_task=task->father_ptr;
	task->state=TASK_ZOMBIE;

	printk("task %d exit with exitcode %d\n",task->pid,exitcode);
	get_free_page_info();
	wake_up(&task->father_ptr);
	schedule();
}

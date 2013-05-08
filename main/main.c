#include <lib.h>
#include <io.h>
#include <intr.h>
#include <sched.h>
#include <memory.h>
#include <tty.h>
extern void initial_sched();
extern void initial_display();
extern void initial_memory();
extern void initial_page();
extern void initial_tty();
extern void get_mem_info();
extern void cancle_map();
extern int fork(void *entry, int count);

void init();

int main()
{
    /* memory be initialized first
     * so that the rest kernel can use linear address */
    initial_memory();

    /* their is a 'ret' in the end of initial_memory()
     * so after it returns,eip turns back to physical address
     * just refresh it */
    refresh_eip();

    /* from now on kernel should not need the first 4M of linear address
     * mapped to 4M of physical address. kernel should user linear address
     * of 0x20000000 */
    cancle_map();

    initial_display();
    get_mem_info();
    initial_page();
    initial_tty();
    initial_intr();
    initial_sched();

    printk("Welcome to my own operating system!\n\n");

//	sti();
//	move_to_user_mode();
    fork((void*)0x20105000, PAGE_SIZE);
    fork((void*)0x20105000, PAGE_SIZE);
    fork((void*)0x20105000, PAGE_SIZE);
    fork((void*)0x20105000, PAGE_SIZE);
    fork((void*)0x20105000, PAGE_SIZE);
    fork((void*)0x20105000, PAGE_SIZE);

    for(int i = 2; i < 1 + TTY_NR; i++)
        tasks[i]->tty = ttys[i - 1];

    sti();
    for(;;) {
        hlt();
        schedule();
    }
}


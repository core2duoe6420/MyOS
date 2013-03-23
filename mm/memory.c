#include <io.h>
#include <lib.h>
#include <memory.h>
#include <string.h>

extern unsigned long new_gdt,new_idt;

uint32 total_mem=0;

void get_mem_info(){
	total_mem=*(uint32*)__va(0x7c00)+1024;
	if(total_mem > 512*1024)
		total_mem=512*1024;
	printk("Total Memory:%d KB\n\n",total_mem);
}

void set_page_structure(struct page_structure_struct* ps,int present,int read_write,int user_super,uint32 page_base){
	ps->present=present & 1;
	ps->read_write=read_write & 1;
	ps->user_super=user_super & 1;
	ps->accessed=0;
	ps->dirty=0;
	ps->reserved_a=0;
	ps->reserved_b=0;
	ps->page_base=page_base >> PAGE_SHIFT;
}

void cancle_map(){
	set_page_structure(PDE_ENTRY->pte,0,0,0,0);
}

void set_pde_pte(){
	/* pde and pte user_supervisor bit must both be 1 to let the ring3 access
	 * either one of two is 0 forid it */
	for(int i=0;i<128;i++)
		set_page_structure(PDE_ENTRY->pte+i,0,0,0,0);

	/* map the first pde 4M to pa 0-4M for the kernal to initial */
	set_page_structure(PDE_ENTRY->pte,1,1,0,PDE_START-PAGE_SIZE);
	for(int j=0;j<1024;j++)
		set_super_pte(PDE_ENTRY,0,j,j*0x1000);

	for(int i=128;i<256;i++){
		set_page_structure(PDE_ENTRY->pte+i,1,1,0,PTE_START+(i-128)*PAGE_SIZE);
		for(int j=0;j<1024;j++)
			set_super_pte(PDE_ENTRY,i,j,(i-128)*0x400000+j*0x1000);
	}
}

/* it's strange that gdt idt tss ldt need paging
 * so we have to reload gdt & idt after switch on paging
 * new_gdt & new idt are defined at boot/setup.asm */
#define reload_gdt_idt() \
do{ \
	__asm__("lgdt %0"::"m"(new_gdt)); \
	__asm__("lidt %0"::"m"(new_idt)); \
}while(0)

void initial_memory(){
	set_pde_pte();
	open_paging();
	reload_gdt_idt();
}



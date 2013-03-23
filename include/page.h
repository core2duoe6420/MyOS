#ifndef __PAGE_H
#define __PAGE_H

#include <const.h>
#include <lib.h>

#define PAGE_KERNEL 0x80000000 /* for kernel */
#define PAGE_PDE 0x40000000 /* for pde */ 
#define PAGE_PTE 0x20000000 /* for pte */
#define PAGE_TASK_STRUCT 0x10000000 /* for task struct */
#define PAGE_TASK_CODE 0x08000000
#define PAGE_TASK_DATA 0x04000000
#define PAGE_TASK_STACK 0x02000000
#define PAGE_DATASTRUCT 0x01000000
#define PAGE_TTY 0x00800000

#define page_to_index(page) \
	((uint32)((page)-PAGE_STRUCT_ENTRY))
#define page_to_address(page) \
	(__va((page_to_index(page)<<PAGE_SHIFT)))
#define address_to_index(address) \
	((__pa((address)))>>PAGE_SHIFT)
#define address_to_page(address) \
	(PAGE_STRUCT_ENTRY+address_to_index(address))

/* the following two macro can only be used in kernel code 
 * for kernel page table is continuous */
#define clear_present(index) \
do{ \
	*((int*)(__va(PTE_START)+sizeof(struct page_structure_struct)*(index)))\
	 &= 0xfffffffe;\
}while(0)

#define set_present(index) \
do{ \
	*((int*)(__va(PTE_START)+sizeof(struct page_structure_struct)*(index)))\
	 |= 1;\
}while(0)


struct page_struct{
	uint32 flags;
	uint32 count;
	struct page_struct *next;
	struct page_struct *prev;
};

struct pagelist{
	struct page_struct *first;
	int nr;
};

struct mm_struct{
	struct pagelist code;
	struct pagelist data;
	struct pagelist stack;
	struct pagelist pde;
	struct pagelist pte;
};

extern struct pagelist freepage;

extern int free_page(struct page_struct* page);
extern struct page_struct * alloc_page(int order,int flags);

inline int get_free_page_info(){
	printk("page available:%d frames\n",freepage.nr);
	return freepage.nr;
}
#endif

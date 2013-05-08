#ifndef __MEMORY_H
#define __MEMORY_H
#include <lib.h>
#include <const.h>

#define __va(address) ((address)+PAGE_OFFSET)
#define __pa(address) ((address)-PAGE_OFFSET)

struct page_structure_struct {
    uint32 present: 1;
    uint32 read_write: 1;
    uint32 user_super: 1;
    uint32 reserved_a: 2;
    uint32 accessed: 1;
    uint32 dirty: 1;
    uint32 reserved_b: 5;
    uint32 page_base: 20;
};

struct pde_struct {
    struct page_structure_struct pte[1024];
};

struct pte_struct {
    struct page_structure_struct pfe[1024];
};

#define get_address_from_ps(ps) ((ps)->page_base << PAGE_SHIFT)

#define set_pte(pde,pde_nr,pte_nr,us,address) \
set_page_structure((struct page_structure_struct*)get_address_from_ps((pde)->pte+(pde_nr))+(pte_nr),1,1,us,(address));

#define set_super_pte(pde,pde_nr,pte_nr,address) \
set_pte(pde,pde_nr,pte_nr,0,address)

#define set_user_pte(pde,pde_nr,pte_nr,address) \
set_pte(pde,pde_nr,pte_nr,1,address)

extern void set_page_structure(struct page_structure_struct* ps, int present, int read_write, int user_super, uint32 page_base);

/* add PAGE_OFFSET to ebp & esp
 * to make sure the esp be linear address from now on
 * however,I'm not sure whether it is proper to do it here
 */
#define open_paging() \
__asm__("addl $0x20000000,%%ebp\n\t" \
	"addl $0x20000000,%%esp\n\t" \
	"movl $0x17f000,%%eax\n\t" \
	"movl %%eax,%%cr3\n\t" \
	"movl %%cr0,%%eax\n\t" \
	"orl $0x80000000,%%eax\n\t" \
	"movl %%eax,%%cr0\n\t" \
	"jmp 1f\n\t" \
	"1:\n\t" \
	"movl $1f,%%eax\n\t" \
	"jmp *%%eax\n\t" \
	"1:"::)


#define refresh_eip() \
__asm__("movl $1f,%%eax\n\t" \
	"jmp *%%eax\n\t" \
	"1:"::)

#endif

#ifndef __CONST_H
#define __CONST_H

/* gdt set in 602K */
#define GDT_PTR ((struct desc_struct*)__va(0x96800))
#define LDT_ENTRY_PTR (GDT_PTR+3)
#define TSS_ENTRY_PTR (GDT_PTR+4)
/* idt set in 604K */
#define LDT_PTR ((struct desc_struct*)__va(0x97000))
/* idt set in 600K */
#define IDT_PTR (__va(0x96000))

/* for set_intr_item;set_trap_item;set_system_item*/
#define TYPE_INT 0xE
#define TYPE_TRAP 0xF

/* for schedule */
#define TASK_MAX_NR 64
#define TASK_RUNNING 0
#define TASK_INTERRUPTIBLE 1
#define TASK_UNINTERRUPTIBLE 2
#define TASK_STOPPED 3
#define TASK_ZOMBIE 4

/* for paging */
/* NOTE: only pde and pte address is physical address
 * EVERYTHING else including GDT IDT LDT TSS base address
 * need translation after switch on PAGING */
#define MAP_SIZE (512*1024*1024)
#define MAP_PAGE_NR (MAP_SIZE>>22)
#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
#define PAGE_OFFSET 0x20000000
/* pde start at 1.5MB-PAGE_SIZE*/
/* if this is changed, remember to change
 * open_paging()
 */
#define PDE_START (0x180000-PAGE_SIZE)
#define PDE_ENTRY ((struct pde_struct*)PDE_START)
/* pte start at 1.5MB length 512KB 128 items*/
#define PTE_START 0x180000
#define PTE_ENTRY ((struct pte_struct*)PTE_START)

/* for tasks' memory layout */
#define TASK_ENTRY 0x8000000
#define TASK_STACK 0x10000000

/* page_struct starts at 2M */
#define PAGE_STRUCT_START (__va(0x200000))
#define PAGE_STRUCT_ENTRY (struct page_struct*)PAGE_STRUCT_START
#define PAGE_STRUCT_SIZE (sizeof(struct page_struct)*PAGE_NR)
#define PAGE_STRUCT_END (PAGE_STRUCT_START+PAGE_STRUCT_SIZE-1) 


#endif

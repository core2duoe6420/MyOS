#include <page.h>
#include <memory.h>
#include <sched.h>

extern uint32 total_mem;

int PAGE_NR;
struct pagelist freepage;

void initial_page()
{
    struct page_struct *p = PAGE_STRUCT_ENTRY;
    PAGE_NR = total_mem >> 2;
    int b_idx, e_idx;

    b_idx = 0;
    e_idx = __pa(PAGE_STRUCT_END) >> 12;

    /* physical memory too little. we have to die here */
    if(e_idx > PAGE_NR) {
        printk("Physical memory doesn't meet system minimal requirement.");
        for(;;);
    }

    for(int i = b_idx; i <= e_idx; i++) {
        p[i].next = p + (i + 1);
        p[i].prev = p - (i - 1);
        p[i].count = 1;
        p[i].flags |= PAGE_KERNEL;
    }

    p[b_idx].prev = NULL;
    p[e_idx].next = NULL;

    /* kernel pde & pte model */
    p[PDE_START >> 12].flags |= PAGE_PDE;
    for(int i = PTE_START >> 12; i < ((PTE_START + MAP_PAGE_NR) >> 12); i++)
        p[i].flags |= PAGE_PTE;

    b_idx = e_idx + 1;
    e_idx = PAGE_NR - 1;

    for(int i = b_idx; i <= e_idx; i++) {
        p[i].next = p + (i + 1);
        p[i].prev = p + (i - 1);
        p[i].count = 0;
        p[i].flags = 0;
        /* pages unused are set unpresent to protect kernel
         * basic info (base address,etc.)has been set
         * when memory is initialized
         * it will be set to present when is allocated */
        clear_present(i);
    }

    p[b_idx].prev = NULL;
    p[e_idx].next = NULL;

    freepage.first = p + b_idx;
    freepage.nr = e_idx - b_idx + 1;

    printk("page_struct size:%d B\n", PAGE_STRUCT_SIZE);
}

struct page_struct * alloc_page(int order, int flags)
{
    int n = 1 << order;
    struct page_struct *start, *end;

    /* oom :( */
    if(n > freepage.nr) {
        printk("Out of memory!\n");
        return NULL;
    }

    start = end = freepage.first;
    for(int i = 0; i < n; i++) {
        end->count++;
        end->flags |= flags;
        /* now the page is present */
        set_present(page_to_index(end));
        end = end->next;
    }

    end = end->prev;
    freepage.first = end->next;
    freepage.nr -= n;

    start->prev = NULL;
    end->next = NULL;

    return start;
}

int free_page(struct page_struct* page)
{
    struct page_struct *start, *end, *last = NULL;
    int n;

    start = end = page;

    if(!start->count) {
        printk("Trying to free unused page.\n");
        return 0;
    }

    if(page_to_index(start) <= __pa(PAGE_STRUCT_END) >> 12) {
        printk("Trying to free reserved page.\n");
        return 0;
    }

    if(page->prev) {
        printk("Trying to free middle page.\n");
        return 0;
    }

    for(n = 0; end; n++) {
        end->flags = 0;
        end->count--;
        clear_present(page_to_index(end));
        last = end;
        end = end->next;
    }
    end = last;
    /* count!=0 means someone else refers to these pages */
    if(start->count)
        return 0;

    end->next = freepage.first;
    freepage.first->prev = end;
    freepage.first = start;
    freepage.nr += n;

    return n;
}

#include <string.h>
#include <intr.h>
#include <lib.h>
#include <io.h>
#include <sched.h>
#include <memory.h>

extern int exit(int exitcode);
extern void do_signal();

/* interrupt */
extern void general_intr(void);
extern void keyboard_intr(void);
extern void timer_intr(void);

/* traps */
extern void divide_error(void);
extern void debug(void);
extern void nmi(void);
extern void int3(void);
extern void overflow(void);
extern void bounds(void);
extern void invalid_op(void);
extern void device_not_available(void);
extern void double_fault(void);
extern void coprocessor_segment_overrun(void);
extern void invalid_TSS(void);
extern void segment_not_present(void);
extern void stack_segment(void);
extern void general_protection(void);
extern void page_fault(void);
extern void reserved(void);
extern void coprocessor_error(void);
extern void alignment_check(void);

/* system call */
extern void sys_call(void);

struct idt_item{
	uint32 base_low:16;
	uint32 segment:16;
	uint32 reserved:8;
	uint32 type:5;
	uint32 dpl:2;
	uint32 p:1;
	uint32 base_high:16;
};

void set_idt_item(void* address,int intr_n,int type,int dpl){
	uint32 addr_idt;
	struct idt_item idt;
	addr_idt=IDT_PTR+(intr_n<<3);
	idt.base_low=(uint32)address & 0xFFFF;
	idt.segment=0x8;
	idt.reserved=0;
	idt.type=type;
	idt.dpl=dpl;
	idt.p=1;
	idt.base_high=(uint32)address>>16;
	memcpy((void*)addr_idt,&idt,sizeof(struct idt_item));
}

static void initial_8259_8254(){
	outb_p(0x11,0x20);
	outb_p(0x20,0x21);
	outb_p(0x04,0x21);
	outb_p(0x01,0x21);
	outb_p(0x11,0xA0);
	outb_p(0x28,0xA1);
	outb_p(0x02,0xA1);
	outb_p(0x01,0xA1);
	outb_p(0x36,0x43);
	outb_p(65535&0xff,0x40);
	outb_p(65535>>8,0x40);
}

static void die(char* info,uint32 esp_ptr,int err){
	uint32 *esp=(uint32*)esp_ptr;
	printk("\n*****************\n%s,error code:%d\n",info,err);
	printk("Context register:\n");
	printk("eax:%08x ebx:%08x ecx:%08x edx:%08x\n",
		esp[10],esp[9],esp[8],esp[7]);
	printk("esi:%08x edi:%08x ebp:%08x\n",
		esp[6],esp[5],esp[4]);
	printk("ds:%04x es:%04x gs:%04x fs:%04x\n",
		esp[3]&0xffff,esp[2]&0xffff,esp[1]&0xffff,esp[0]&0xffff);
	
	uint32 cr2,cr3;
	__asm__("movl	%%cr2,%%eax":"=a"(cr2):);
	__asm__("movl	%%cr3,%%eax":"=a"(cr3):);
	printk("cr2:%08x cr3:%08x\n",cr2,cr3);
	printk("eip:%08x cs:%04x\n",esp[11],esp[12]&0xffff);
	printk("eflags:%08x\n",esp[13]);
	if(esp[12]&0x03 == 3)
		printk("origin esp:%08x ss:%04x\n",esp[14],esp[15]&0xffff);
	exit(1);
}

void do_general_intr(){
	printk("an interruput\n");
}

void do_timer_intr(){
	static int i=0;
	if(i++<2)
		return;
	i=0;
	current->counter--;
	schedule();
	do_signal();
}

void do_int3(uint32 esp,int errcode){
	die("int3",esp,errcode);
}

void do_general_protection(uint32 esp,int errcode){
	die("General protection",esp,errcode);
}

void do_page_fault(uint32 esp,int errcode){
	die("Page fault",esp,errcode);
}

void do_divide_error(uint32 esp,int errcode){
	die("Divide error",esp,errcode);
}

void do_nmi(uint32 esp,int errcode){
	die("Nmi",esp,errcode);
}

void do_debug(uint32 esp,int errcode){
	die("Debug",esp,errcode);
}

void do_overflow(uint32 esp,int errcode){
	die("Overflow",esp,errcode);
}

void do_bounds(uint32 esp,int errcode){
	die("Bounds",esp,errcode);
}

void do_invalid_op(uint32 esp,int errcode){
	die("Invalid operand",esp,errcode);
}

void do_device_not_available(uint32 esp,int errcode){
	die("Device not available",esp,errcode);
}

void do_coprocessor_segment_overrun(uint32 esp,int errcode){
	die("Coprocessor segment overrun",esp,errcode);
}

void do_invalid_TSS(uint32 esp,int errcode){
	die("Invalid TSS",esp,errcode);
}

void do_segment_not_present(uint32 esp,int errcode){
	die("Segment not present",esp,errcode);
}

void do_stack_segment(uint32 esp,int errcode){
	die("Stack segment",esp,errcode);
}

void do_double_fault(uint32 esp,int errcode){
	die("Double fault",esp,errcode);
}

void do_coprocessor_error(uint32 esp,int errcode){
	die("Coprocessor error",esp,errcode);
}

void do_reserved(uint32 esp,int errcode){
	die("Reserved (15,17-47) error",esp,errcode);
}

void do_alignment_check(uint32 esp,int errcode){
	die("Alignment check",esp,errcode);
}

void initial_intr(){
	initial_8259_8254();
	for(int i=18;i<48;i++)
		set_intr_item(general_intr,i);

	/* traps */
	set_trap_item(divide_error,0);
	set_trap_item(debug,1);
	set_trap_item(nmi,2);
	set_system_item(int3,3);	/* int3-5 can be called from all */
	set_system_item(overflow,4);
	set_system_item(bounds,5);
	set_trap_item(invalid_op,6);
	set_trap_item(device_not_available,7);
	set_trap_item(double_fault,8);
	set_trap_item(coprocessor_segment_overrun,9);
	set_trap_item(invalid_TSS,10);
	set_trap_item(segment_not_present,11);
	set_trap_item(stack_segment,12);
	set_trap_item(general_protection,13);
	set_trap_item(page_fault,14);
	set_trap_item(reserved,15);
	set_trap_item(coprocessor_error,16);
	set_trap_item(alignment_check,17);

	/* interrupt */
	set_intr_item(timer_intr,0x20);
	set_intr_item(keyboard_intr,0x21);

	/* system call */
	set_system_item(sys_call,0x30);
}



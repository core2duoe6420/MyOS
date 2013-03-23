#ifndef __INTR_H
#define __INTR_H

#include <const.h>

#define set_intr_item(address,intr_n) \
	set_idt_item(address,intr_n,TYPE_INT,0);

#define set_trap_item(address,intr_n)\
	set_idt_item(address,intr_n,TYPE_TRAP,0);

#define set_system_item(address,intr_n)\
	set_idt_item(address,intr_n,TYPE_TRAP,3);

void initial_intr();


#endif

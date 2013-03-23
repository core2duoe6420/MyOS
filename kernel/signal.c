#include <signal.h>
#include <sched.h>
#include <lib.h>

typedef void (*sigaction_f)();

extern int exit(int exitcode);

void do_sig_kill(){
	exit(2);
}

sigaction_f sigaction[33]={
	NULL,
	do_sig_kill,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
};

inline int get_signal(uint32 mask){
	int retval=0;
	while(!((mask>>retval) & 0x1))
		retval++;

	return retval+1;
}

void do_signal(){
	int sig;
	if(current==tasks[0] || (!current->signal))
		return;
	sig=get_signal(current->signal);
	printk("\nSIG %d received\n",sig);
	if(sig!=SIG_KILL){
		if((1<<(sig-1))&current->sig_block){
			printk("\nSIG %d blocked\n",sig);
			clear_signal(current->signal,sig);
			return;
		}
	}
	sigaction[sig]();
}



CFLAGS=cc -m32 -c -nostdinc -I../include -fno-builtin -fno-stack-protector -std=gnu99 -O -W -Werror
ASMFLAGS=nasm -f elf
LIST=printk.o vsprintf.o intr.o intr_entry.o sched.o fork.o exit.o signal.o
kernel:$(LIST)
	ld $(LIST) -melf_i386 -r -o kernel.o
printk.o:printk.c ../include/*.h
	$(CFLAGS) printk.c
vsprintf.o:vsprintf.c ../include/*.h
	$(CFLAGS) vsprintf.c
intr.o:intr.c ../include/*.h
	$(CFLAGS) intr.c
sched.o:sched.c ../include/*.h
	$(CFLAGS) sched.c
fork.o:fork.c ../include/*.h
	$(CFLAGS) fork.c
exit.o:exit.c ../include/*.h
	$(CFLAGS) exit.c
signal.o:signal.c ../include/*.h
	$(CFLAGS) signal.c
intr_entry.o:intr_entry.asm
	$(ASMFLAGS) intr_entry.asm
clean:
	rm -f *.o *~

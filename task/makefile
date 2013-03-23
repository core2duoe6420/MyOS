CFLAGS=cc -m32 -c -nostdinc -I. -fno-builtin -fno-stack-protector -finline-functions -fgnu89-inline -std=gnu99 -O -W -Werror
LDFLAGS=ld -melf_i386 -T lds

test:task1.o task2.o init.o printf.o
	$(LDFLAGS) init.o printf.o -o init.bin -Map=init.lst
	$(LDFLAGS) task1.o -o task1.bin -Map=task1.lst
	$(LDFLAGS) task2.o -o task2.bin -Map=task2.lst
init.o:init.c
	$(CFLAGS) init.c
task1.o:task1.c
	$(CFLAGS) task1.c
task2.o:task2.c
	$(CFLAGS) task2.c
printf.o:printf.c *.h
	$(CFLAGS) printf.c
clean:
	rm -f *.o *.bin *~ *.lst

CFLAGS=cc -m32 -c -nostdinc -I../include -fno-builtin -fno-stack-protector -std=gnu99 -O -W -Werror
ASMFLAGS=nasm -f elf
LIST=memory.o page.o
mm:$(LIST)
	ld $(LIST) -melf_i386 -r -o mm.o
memory.o:memory.c ../include/*.h
	$(CFLAGS) memory.c
page.o:page.c ../include/*.h
	$(CFLAGS) page.c
clean:
	rm -f *.o *~

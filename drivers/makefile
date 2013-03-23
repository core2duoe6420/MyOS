CFLAGS=cc -m32 -c -nostdinc -I../include -fno-builtin -fno-stack-protector -std=gnu99 -O -W -Werror
ASMFLAGS=nasm -f elf
LIST=keyboard.o kb_entry.o tty.o
mm:$(LIST)
	ld $(LIST) -melf_i386 -r -o drivers.o
keyboard.o:keyboard.c ../include/*.h
	$(CFLAGS) keyboard.c
tty.o:tty.c ../include/*.h
	$(CFLAGS) tty.c
kb_entry.o:kb_entry.asm
	$(ASMFLAGS) kb_entry.asm
clean:
	rm -f *.o *~

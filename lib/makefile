CFLAGS=cc -m32 -c -nostdinc -I../include -fno-builtin -fno-stack-protector -std=gnu99 -O -W -Werror
ASMFLAGS=nasm -f elf
LIST=datastruct.o
mm:$(LIST)
	ld $(LIST) -melf_i386 -r -o lib.o
datastruct.o:datastruct.c ../include/*.h
	$(CFLAGS) datastruct.c
clean:
	rm -f *.o *~

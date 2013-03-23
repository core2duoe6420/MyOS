CFLAGS=cc -m32 -c -nostdinc -I../include -fno-builtin -fno-stack-protector -std=gnu99 -O -W -Werror
LIST=main_part.o

start:$(LIST)
	ld $(LIST) -melf_i386 -r -o main.o
main_part.o:main.c ../include/*.h
	$(CFLAGS) main.c -o main_part.o
clean:
	rm -f *.o *~

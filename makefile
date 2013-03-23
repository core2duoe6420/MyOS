DEST=img.bin
LIST=boot/setup.o main/main.o kernel/kernel.o mm/mm.o drivers/drivers.o lib/lib.o

total:$(LIST) boot/boot.bin
	ld $(LIST) -o $(DEST) -melf_i386 -Map=list.lst -T lds;make img
boot/setup.o:boot/setup.asm
	cd boot;nasm -f elf setup.asm
boot/boot.bin:boot/boot.asm
	cd boot;nasm boot.asm -o boot.bin
main/main.o:main/*.c include/*.h
	cd main;make
kernel/kernel.o:kernel/*.c include/*.h
	cd kernel;make
mm/mm.o:mm/*.c include/*.h
	cd mm;make
drivers/drivers.o:drivers/*.c drivers/*.asm include/*.h
	cd drivers;make
lib/lib.o:lib/*.c include/*.h
	cd lib;make
img:
	cd task;make
	./mimg.out -o floppy.img boot/boot.bin $(DEST);
	./mimg.out -o floppy.img task/init.bin -f 20992
	./mimg.out -o floppy.img task/task1.bin -f 25088
	./mimg.out -o floppy.img task/task2.bin -f 29184
show:
	ndisasm -b 32 $(DEST) | more
clean:
	rm -f *.o *.bin *.lst *.img *~;cd kernel;make clean;cd ../main;make clean;cd ../task;make clean;cd ../mm;make clean;cd ../drivers;make clean

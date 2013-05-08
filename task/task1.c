__attribute__((__section__(".start"))) void main()
{
    volatile char a[] = "B";
    volatile int i = 0;
    for(;;) {
        i++;
        if(i > 5000000) {
            __asm__("int $0x30"::"a"(3), "b"(a));
            i = 0;
        }
    }
}

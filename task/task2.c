__attribute__((__section__(".start")))  void main(){
	__asm__("int $0x30"::"a"(1));
	for(;;);
}

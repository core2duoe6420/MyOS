int strlen(const char *str);
int strcmp(const char *a,const char *b);
void fork(void *address,int count);
void gets(char *address);
void pause();

extern int printf(const char *fmt, ...);

__attribute__((__section__(".start"))) void main() {
	char buffer[1024];
	int new_task;
	for(;;){
		new_task=1;
		gets(buffer);
		if(!strcmp(buffer,"testa")){
			fork((void*)0x20106000,4096);
		}else if(!strcmp(buffer,"testb")){
			fork((void*)0x20107000,4096);
		}else{
			printf("No task available.\n");
			new_task=0;
		}
		if(new_task)
			pause();
	}	
}

void fork(void* address,int count){
	__asm__("int $0x30"
		::"a"(0),"b"(address),"c"(count));
}

void gets(char *address){
	__asm__("int $0x30"
		::"a"(2),"b"(address));
}

void pause(){
	__asm__("int $0x30"
		::"a"(4));
}

int strlen(const char *str){
	int i=0;
	while(str[i++]);
	return i-1;
}

int strcmp(const char *a,const char *b){
	int lena,lenb;
	lena=strlen(a);
	lenb=strlen(b);
	if(lena!=lenb)
		return 1;
	for(int i=0;i<lena;i++)
		if(a[i]!=b[i])
			return 1;
	return 0;
}



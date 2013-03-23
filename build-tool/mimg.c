#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define IMG_SIZE (1440*1024)
#define DEFAULT_FILENAME "image.img"

#define ERR_SOURCE 100
#define ERR_IMGSIZEOVER 101
#define ERR_READ 102
#define ERR_WRITE 103
#define ERR_ARGW 104
#define ERR_OPENIMG 105

#define err_exit(code) \
	do{\
	printf("Error code:%d",-code);\
	exit(code);\
	}while(0)

int write_buffer(char *from,char *dbuffer,int offset){
	int source_len;
	char *sbuffer;
	FILE *source;
	int read,write;

	source=fopen(from,"rb");
	if(!source)
		return -ERR_SOURCE;
	fseek(source,0,SEEK_SET);
	fseek(source,0,SEEK_END);
	source_len=ftell(source);
	fseek(source,0,SEEK_SET);

	if(source_len+offset>IMG_SIZE)
		return -ERR_IMGSIZEOVER;

	sbuffer=(char*)malloc(source_len+1);
	read=fread(sbuffer,1,source_len,source);
	if(read!=source_len)
		return -ERR_READ;
	fclose(source);
	memcpy(dbuffer+offset,sbuffer,source_len);
	free(sbuffer);
	return source_len;
}

int read_img(char *img,char *buffer){
	FILE *fimg;
	int read;
	if(access(img,0))
		return 0;
	fimg=fopen(img,"rb");
	if(!fimg)
		return -ERR_OPENIMG;
	read=fread(buffer,1,IMG_SIZE,fimg);
	if(read!=IMG_SIZE)
		return -ERR_READ;
	fclose(fimg);
	return read;
}

int write_img(char *img_name,char *buffer){
	FILE *img;
	int write;
	img=fopen(img_name,"wb");
	if(!img)
		return -ERR_OPENIMG;
	write=fwrite(buffer,1,IMG_SIZE,img);
	if(write!=IMG_SIZE)
		return -ERR_WRITE;
	fclose(img);
	return 0;
}

int main(int argc,char *argv[]){
	int err=0;
	char *img_name;
	char bin[32][128];
	int bin_nr=0;
	int offset=0;
	int total_w=0;
	char *buffer;

	if(argc==1)
		exit(0);

	img_name=DEFAULT_FILENAME;
	for(int i=1;i<argc;i++){
		if(strcmp(argv[i],"-o")==0){
			if(!argv[i+1]){
				err=-ERR_ARGW;
				break;
			}
			img_name=argv[i+1];
			i++;
		}else if(strcmp(argv[i],"-f")==0){
			if(!argv[i+1]){
				err=-ERR_ARGW;
				break;
			}
			offset=atoi(argv[i+1]);
			i++;
		}else{
			strcpy(bin[bin_nr],argv[i]);
			bin_nr++;
		}
	}
	if(err)
		err_exit(err);
	buffer=(char*)calloc(IMG_SIZE,1);
	if((err=read_img(img_name,buffer))<0)
		err_exit(err);

	for(int i=0;i<bin_nr;i++){
		int res;
		res=write_buffer(bin[i],buffer,offset);
		if(res<0)
			err_exit(res);
		offset+=res;
		total_w+=res;
	}
	if(err=write_img(img_name,buffer))
		err_exit(err);
	free(buffer);
	printf("%d bytes written.\n",total_w);
}

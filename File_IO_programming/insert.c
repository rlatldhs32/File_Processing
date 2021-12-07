#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[])
{
	int fd;
	int i;
	int offset;
	int len, f_size;
	char *buf1, *buf2;
	char *space = " ";
	int test1, test2;

	offset = atoi(argv[2]);

	if((fd = open(argv[1], O_RDWR))<0){
		fprintf(stderr,"error openr for %s!!\n", argv[1]);
		exit(1);
	}

	lseek(fd, 0, SEEK_SET);
	f_size = lseek(fd,0,SEEK_END);
	lseek(fd,0,SEEK_SET);
	lseek(fd,offset,SEEK_CUR);

	buf1 = (char *)malloc(f_size);

	len = read(fd,buf1,f_size-offset);
	buf1[len]=0;

	lseek(fd,0,SEEK_SET);
	lseek(fd,offset+1,SEEK_CUR);
	
	for(i=3;i<argc;i++){
		write(fd,argv[i],strlen(argv[i]));
		if(i == argc-1)
			break;
		else
			write(fd,space,1);
	}
	write(fd,buf1,len);
	
	exit(0);
}





/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 10
int main(int argc,char* argv[]){
	int offset;
	offset=atoi(argv[2]);
	int fd,tmpfd;
	int FileSize,Addsize;
	char * buf;
	char * blank = " ";
	
	if((fd=open(argv[1],O_WRONLY))==-1)
	{		
		fprintf(stderr,"Error happened when opening %s.\n",argv[1]);
		return -1;
	}
	
	FileSize=lseek(fd,0,SEEK_END);
	buf=(char*)malloc(FileSize);
	memset(buf,0,FileSize);
	lseek(fd,offset+1,SEEK_SET);
	
	

	Addsize=read(fd,buf,FileSize-offset);

	lseek(fd,offset,SEEK_SET);
	
	for(int i=3;i<argc;i++)
	{
		write(fd,argv[i],strlen(argv[i]));
		if(i==argc-1)
			break;
		else
			write(fd,blank,1);
	}
	write(fd,buf,Addsize);
}*/

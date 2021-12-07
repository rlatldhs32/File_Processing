#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc,char * argv[])
{
	int offset,byte;
	offset=atoi(argv[2]);
	byte=atoi(argv[3]);
	char * buff;
	int fd;
	int count;

	buff=(char*)malloc(byte);
	if((fd=open(argv[1],O_RDONLY))==-1)
	{
		fprintf(stderr,"Error happen when opening %s",argv[1]);
		return -1;
	}
	lseek(fd,offset,SEEK_CUR);

	count=read(fd,buff,byte);

	printf("context :  %s " ,buff);
}


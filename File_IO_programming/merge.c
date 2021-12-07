#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 10

int main(int argc,char * argv[])
{
	int fd1,fd2,fd3;
	int length;
	char buf[BUFFER_SIZE];
	char buf2[BUFFER_SIZE];
	if((fd1=open(argv[1],O_WRONLY|O_CREAT|O_TRUNC,0644))==-1)
	{
		fprintf(stderr,"Opening/Creating Error for  %s\n",argv[1]);
		return -1;
	}
	if((fd2=open(argv[2],O_RDONLY))==-1)
	{
	 	fprintf(stderr,"Opening Error for %s\n",argv[2]);
	 	return -1;
	}
	if((fd3=open(argv[3],O_RDONLY))==-1)
	{
	 	fprintf(stderr,"Opening Error for %s\n",argv[3]);
	 	return -1;
	}

	lseek(fd1,0,SEEK_SET);
	
	while((length=read(fd2,buf,BUFFER_SIZE))>0)
		write(fd1,buf,length);

	while((length=read(fd3,buf2,BUFFER_SIZE))>0)
		write(fd1,buf2,length);
	return 0;
}



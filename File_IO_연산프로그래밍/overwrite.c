#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 100
int main(int argc,char* argv[])
{
	int offset;
	int fd,i;
	char buf[BUFFER_SIZE];
	char * blank =" ";

	offset=atoi(argv[2]);

	if((fd=open(argv[1],O_WRONLY))==-1)
	{
		fprintf(stderr,"Error happen when open %s\n",argv[1]);
		return -1;
	}

	lseek(fd,0,SEEK_SET);
	lseek(fd,offset,SEEK_CUR);
	
	
	for(i=3;i<argc;i++)
	{	
		write(fd,argv[i],strlen(argv[i]));
		if(i==argc-1)
			break;
		else
			write(fd,blank,1);
	}
	return 0;
	

}

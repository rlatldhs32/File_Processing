#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[])
{
	int offset, byte;
	int fd,len,f_size,count;
	char *buf;

	offset = atoi(argv[2]);
	byte = atoi(argv[3]);

	if((fd = open(argv[1],O_RDWR))==-1)
	{
		fprintf(stderr,"Error happened when opening %s.\n",argv[1]);
		return -1;
	}
	
	
	f_size = lseek(fd,0,SEEK_END);
	buf=(char*)malloc(f_size);
	lseek(fd,0,SEEK_SET);
	
	if(byte > (f_size-offset))
		ftruncate(fd,offset);
	else
	{
		len = offset+byte;
		lseek(fd,len,SEEK_CUR);
		len = f_size - len;
		count = read(fd,buf,len); // Read last strings.

		lseek(fd,offset,SEEK_SET);
		write(fd,buf,count);
		ftruncate(fd, f_size-byte);	
		return 0;
	}
}




/*#include <stdio.h>



int main(int argc,char* argc[])
{
	int fd;
	int offset;
	int Dbyte;
	offset=atoi(argc[2]);
	Dbyte=atoi(argc[3]);
	int FileSize;

	if((fd=open(argv[1],O_WRONLY))==-1)
	{
		fprintf(stderr,"Error happened when open %s.\n",argv[1]);
		return -1;
	}
	FileSize=lseek(fd,0,SEEK_END);

	if( (Dbyte+offset) > FileSize)
	{
		Dbyte=FileSize-offset;
		lseek(fd,0,SEEK_SET);
		ftruncate(fd,offset);
	}
	else
	{
		

	}
	{






}
*/

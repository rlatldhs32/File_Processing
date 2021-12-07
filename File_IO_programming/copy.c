#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE  10
#define S_MODE  0644
int main(int argc, char * argv[]) {
   int fd1,fd2;
   char buf[BUFFER_SIZE];
   int length;
   if (argc != 3)
   {
	   fprintf(stderr, "Error happened when reading and copying : %s\n", argv[0]);
	   return -1;
   }

	if ((fd1 = open(argv[1], O_RDONLY)) == -1) 
	{
			fprintf(stderr, "Error happened when opening %s.\n",argv[1] );
		    return -1;
	}
	if ((fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
	{
		fprintf(stderr, "Error happend when writing %s.\n ",argv[2]);
		return -1;
	}
	lseek(fd1, 0, SEEK_SET);

	while ((length = read(fd1, buf, BUFFER_SIZE)) > 0)
	{
		write(fd2, buf, length);
	}
	   return 0;
	
}

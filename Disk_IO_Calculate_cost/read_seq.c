#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
//필요하면 header file 추가 가능

int main(int argc, char** argv)
{
	int fd;
	int fileSize;
	int RecordNum;
	int Result;
	fd = open(argv[1], O_RDONLY);
	char buf[250];
	struct timeval before, after, result;

	read(fd, &RecordNum, sizeof(int));

	lseek(fd, 4, SEEK_SET);

	gettimeofday(&before, NULL);

	for (int i = 0; i < RecordNum; i++)
		read(fd, buf, 250);

	gettimeofday(&after, NULL);
	Result = (after.tv_sec - before.tv_sec) * 1000000 + after.tv_usec - before.tv_usec;

	printf("records: %d elapsed_time: %d us\n", RecordNum, Result);
	close(fd);
	return 0;
}

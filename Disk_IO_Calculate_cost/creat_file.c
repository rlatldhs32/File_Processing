#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
//필요하면 header file 추가 가능

int main(int argc, char** argv)
{
	FILE* fd;
	int  fd2;
	int num;
	int a;
	char* buf;

	num = atoi(argv[1]);
	fd2 = open(argv[2], O_WRONLY | O_CREAT, 0644);
	write(fd2, &num, sizeof(int));
	buf = "1234567890abcdefgoik1234567890abcdefgoik1234567890abcdefgoik1234567890abcdefgoik1234567890abcdefgoik1234567890abcdefgoik1234567890abcdefgoik1234567890abcdefgoik1234567890abcdefgoik1234567890abcdefgoik1234567890abcdefgoik1234567890abcdefgoik1234567890";
	write(fd2, buf, num * 250);


	close(fd2);
	return 0;
}
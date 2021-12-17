#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

//필요하면 header file 추가 가능

#define SUFFLE_NUM	10000	// 이 값은 마음대로 수정할 수 있음.

void GenRecordSequence(int* list, int n);
void swap(int* a, int* b);
// 필요한 함수가 있으면 더 추가할 수 있음.

int main(int argc, char** argv)
{
	int fd;
	int* read_order_list;

	struct timeval before, after;
	char buf[250];
	fd = open(argv[1], O_RDONLY);

	int num_of_records; // 레코드 파일에 저장되어 있는 전체 레코드의 수
	read(fd, &num_of_records, sizeof(int));
	read_order_list = (int*)malloc(sizeof(int) * num_of_records);

	// 이 함수를 실행하면 'read_order_list' 배열에는 읽어야 할 레코드 번호들이 나열되어 저장됨
	GenRecordSequence(read_order_list, num_of_records);

	lseek(fd, 4, SEEK_SET);

	gettimeofday(&before, NULL);

	for (int i = 0; i < num_of_records; i++)
	{
		lseek(fd, read_order_list[i] * 250 + 4, SEEK_SET);
		read(fd, buf, 250);
	}
	gettimeofday(&after, NULL);
	int Result;
	Result = (after.tv_sec - before.tv_sec) * 1000000 + after.tv_usec - before.tv_usec;

	printf("records: %d elapsed_time: %d us\n", num_of_records, Result);
	close(fd);
	free(read_order_list);
	return 0;
}

void GenRecordSequence(int* list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for (i = 0; i < n; i++)
	{
		list[i] = i;
	}

	for (i = 0; i < SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}
}

void swap(int* a, int* b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

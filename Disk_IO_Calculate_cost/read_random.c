#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

//�ʿ��ϸ� header file �߰� ����

#define SUFFLE_NUM	10000	// �� ���� ������� ������ �� ����.

void GenRecordSequence(int* list, int n);
void swap(int* a, int* b);
// �ʿ��� �Լ��� ������ �� �߰��� �� ����.

int main(int argc, char** argv)
{
	int fd;
	int* read_order_list;

	struct timeval before, after;
	char buf[250];
	fd = open(argv[1], O_RDONLY);

	int num_of_records; // ���ڵ� ���Ͽ� ����Ǿ� �ִ� ��ü ���ڵ��� ��
	read(fd, &num_of_records, sizeof(int));
	read_order_list = (int*)malloc(sizeof(int) * num_of_records);

	// �� �Լ��� �����ϸ� 'read_order_list' �迭���� �о�� �� ���ڵ� ��ȣ���� �����Ǿ� �����
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

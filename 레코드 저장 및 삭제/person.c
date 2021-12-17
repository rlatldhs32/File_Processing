#define _CRT_SECURE_NO_WARNINGS
#define recordS  PAGE_SIZE/MAX_RECORD_SIZE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "person.h"
//�ʿ��� ��� ��� ���ϰ� �Լ��� �߰��� �� ����
  // ������������ / ���ڵ� ������
// ���� ������� �����ϴ� ����� ���� �ٸ� �� ������ �ణ�� ������ �Ӵϴ�.
// ���ڵ� ������ ������ ������ ���� �����Ǳ� ������ ����� ���α׷����� ���ڵ� ���Ϸκ��� �����͸� �а� �� ����
// ������ ������ ����մϴ�. ���� �Ʒ��� �� �Լ��� �ʿ��մϴ�.
// 1. readPage(): �־��� ������ ��ȣ�� ������ �����͸� ���α׷� ������ �о�ͼ� pagebuf�� �����Ѵ�
// 2. writePage(): ���α׷� ���� pagebuf�� �����͸� �־��� ������ ��ȣ�� �����Ѵ�
// ���ڵ� ���Ͽ��� ������ ���ڵ带 �аų� ���ο� ���ڵ带 ���ų� ���� ���ڵ带 ������ ����
// ���� readPage() �Լ��� ȣ���Ͽ� pagebuf�� ������ ��, ���⿡ �ʿ信 ���� ���ο� ���ڵ带 �����ϰų�
// ���� ���ڵ� ������ ���� ��Ÿ�����͸� �����մϴ�. �׸��� �� �� writePage() �Լ��� ȣ���Ͽ� ������ pagebuf��
// ���ڵ� ���Ͽ� �����մϴ�. �ݵ�� ������ ������ �аų� ��� �մϴ�.
//
// ����: ������ �������κ��� ���ڵ�(���� ���ڵ� ����)�� �аų� �� �� ������ ������ I/O�� ó���ؾ� ������,
// ��� ���ڵ��� ��Ÿ�����͸� �����ϰų� �����ϴ� ��� ������ ������ ó������ �ʰ� ���� ���ڵ� ������ �����ؼ� ó���Ѵ�.


void ReadHeaderRecord(FILE* fp, char* pagebuf)
{
	fseek(fp, 0, SEEK_SET); //ó���� ���ΰ�
	fread(pagebuf, 1, 16, fp); // �о�帲
}
void WriteHeaderRecord(FILE* fp, char* pagebuf) //�ִ³��� ���� fp��
{
	fseek(fp, 0, SEEK_SET);
	fwrite(pagebuf, 1, 16, fp);
}

//
// ������ ��ȣ�� �ش��ϴ� �������� �־��� ������ ���ۿ� �о �����Ѵ�. ������ ���۴� �ݵ�� ������ ũ��� ��ġ�ؾ� �Ѵ�.
//
void readPage(FILE* fp, char* pagebuf, int pagenum) // 3��°�ִ°� 
{
	fseek(fp, 16, SEEK_SET);
	fseek(fp, pagenum * PAGE_SIZE, SEEK_CUR);
	fread(pagebuf, 1, PAGE_SIZE, fp);
}

//
// ������ ������ �����͸� �־��� ������ ��ȣ�� �ش��ϴ� ���ڵ� ������ ��ġ�� �����Ѵ�. 
// ������ ���۴� �ݵ�� ������ ũ��� ��ġ�ؾ� �Ѵ�.
//
void writePage(FILE* fp, const char* pagebuf, int pagenum)
{
	fseek(fp, 16, SEEK_SET);
	fseek(fp, pagenum * PAGE_SIZE, SEEK_CUR);
	fwrite(pagebuf, 1, PAGE_SIZE, fp);
}




//
// ���ο� ���ڵ带 ������ �� �͹̳ηκ��� �Է¹��� ������ Person ����ü�� ���� �����ϰ�, pack() �Լ��� ����Ͽ�
// ���ڵ� ���Ͽ� ������ ���ڵ� ���¸� recordbuf�� �����. 
// 
void pack(char* recordbuf, const Person* p)
{
	sprintf(recordbuf, "%s#%s#%s#%s#%s#%s#", p->id, p->name, p->age, p->addr, p->phone, p->email);
}

// 
// �Ʒ��� unpack() �Լ��� recordbuf�� ����Ǿ� �ִ� ���ڵ带 ����ü�� ��ȯ�� �� ����Ѵ�.
//
//
void unpack(const char* recordbuf, Person* p)
{
	char* ptr;
	char rec[MAX_RECORD_SIZE];

	strcpy(rec, recordbuf);

	if (rec[0] != '*') {
		ptr = strtok(rec, "#");
		strcpy(p->id, ptr);

		ptr = strtok(NULL, "#");
		strcpy(p->name, ptr);
		ptr = strtok(NULL, "#");
		strcpy(p->age, ptr);
		ptr = strtok(NULL, "#");
		strcpy(p->addr, ptr);
		ptr = strtok(NULL, "#");
		strcpy(p->phone, ptr);
		ptr = strtok(NULL, "#");
		strcpy(p->email, ptr);
	}
}
//�������� �����ϴ� length ���� ���� 
int ExistedLengthN(FILE* fp, int pagenum)
{
	char pagebuf[PAGE_SIZE];
	char PageHeadbuf[HEADER_AREA_SIZE];
	int result=0,tmp=0;
	int length2 = 0;
	int PageRecordN;
	readPage(fp, pagebuf, pagenum); // ������ �а�
	memcpy(&PageRecordN, pagebuf, 4);
	if (PageRecordN == -1|| PageRecordN == 0)
	{
		return 0;
	}
	for (int i = 0; i < PageRecordN; i++) // ���� recordN�� -1�̶��?
	{
		memcpy(&tmp, pagebuf+ (8*(i+1)), 4); //���� �а� 
		result += tmp; // �װ͸�ŭ ����. �ٵ� ������. ���� �����Ȱſ��ٸ�?
	}
	return result;
}

//�������� �����ϴ� ��� length ����
int ExistedHeaderLengthN(FILE* fp, int pagenum)
{
	char pagebuf[PAGE_SIZE];
	int result = 0;
	int PageRecordN=0;
	readPage(fp, pagebuf, pagenum); // ������ �а�

	memcpy(&PageRecordN, pagebuf, 4);
	if (PageRecordN == -1|| PageRecordN == 0)
	{
		return 0;
	}
	else
	result = 4 + 8 * (PageRecordN);

	return result;
}

int LengthOfpr(FILE* fp, int pagenum, int recordnum) //�־��� �������� ���ڵ� �ѹ��� ���̸� ����
{ //0 ���Ի���
	char pagebuf[PAGE_SIZE];
	int result = 0;
	readPage(fp, pagebuf, pagenum); // ������ �а�
	memcpy(&result, pagebuf+8+recordnum*8, 4); //�ű� �ִ� ���ڵ��� length
	return result;
}

int OffsetOfpr(FILE* fp, int pagenum, int recordnum) //�־��� �������� ���ڵ� �ѹ��� ���̸� ����
{
	char pagebuf[PAGE_SIZE];
	int result = 0;
	readPage(fp, pagebuf, pagenum); // ������ �а�
	memcpy(&result, pagebuf + 4 + recordnum * 8, 4); //�ű� �ִ� ���ڵ��� length
	return result;
}

void RenewpageHeader(FILE* fp, char* recordbuf,int pagenum)
{
	char pagebuf[PAGE_SIZE];
	char headbuf[HEADER_AREA_SIZE];
	memset(pagebuf, (char)0xFF, sizeof(pagebuf));
	int recordN;
	int length = strlen(recordbuf);
	int offset = 0;
	readPage(fp, pagebuf, pagenum); //�������� ��������
	memcpy(&recordN, pagebuf, 4); 
	if (recordN == -1|| recordN==0) //ó�� ������
	{
		recordN = 1;
		memcpy(pagebuf+4, &offset, 4); //���� �ں��� 8����Ʈ ������� oofset , length
		memcpy(pagebuf+ 8, &length, 4);//���� �� �������������ؾ���
		memcpy(pagebuf, &recordN, 4);
		writePage(fp, pagebuf, pagenum);
	}
	else
	{
		offset = HEADER_AREA_SIZE+OffsetOfpr(fp, pagenum, recordN-1) + LengthOfpr(fp, pagenum, recordN-1); // ������ + length = ���� offset
		recordN++;
		memcpy(pagebuf + ExistedHeaderLengthN(fp, pagenum), &offset, 4); //���� �ں��� 8����Ʈ ������� oofset , length
		memcpy(pagebuf + ExistedHeaderLengthN(fp, pagenum) +4, &length, 4);//���� �� �������������ؾ���
		memcpy(pagebuf, &recordN, 4);

		writePage(fp, pagebuf, pagenum);
	}
	
}

//
// ���ο� ���ڵ带 �����ϴ� ����� �����ϸ�, �͹̳ηκ��� �Է¹��� �ʵ尪���� ����ü�� ������ �� �Ʒ� �Լ��� ȣ���Ѵ�.
//
void add(FILE* fp, const Person* p)
{

	char recordbuf[MAX_RECORD_SIZE];
	char pagebuf[PAGE_SIZE];
	char Headbuf[16], PageHeadbuf[HEADER_AREA_SIZE]; // ��ó��, ����������
	int pageN;
	int recordN;
	int length, PageRecordN, offset;
	int deleteP, deleteR;

	memset(Headbuf, (char)0xFF, sizeof(Headbuf));
	memset(pagebuf, (char)0xFF, sizeof(pagebuf)); //���������� �ʱ�ȭ
	memset(recordbuf, (char)0xFF, sizeof(recordbuf)); //���ڵ� ����  �ʱ�ȭ

	pack( recordbuf, p ); //����ü�� �����ϱ� , ���ڵ���ۿ� ���� // ���� �˱�
	length = strlen(recordbuf);
	

	ReadHeaderRecord(fp, Headbuf); //���ڵ� ���� // ���� ó���̸�  0 0 -1 -1 �ŵ�?
	

	memcpy(&pageN, Headbuf, 4); //������ �� (�̰� �ٸ� ������ ������ �� �տ� +1�̾���_)
	memcpy(&recordN, Headbuf + 4, 4); //���ڵ�� (�������)
	memcpy(&deleteP, Headbuf + 8, 4); //�����Ҷ� ������
	memcpy(&deleteR, Headbuf + 12, 4);// �����Ҷ� ���ڵ�


	//pageN�̶� recordN �����ؾ���.  ���ο��� 0 0 -1-1�� �־����
	//offset�̶� length�� PageRecordN�� �����ؾ���.!
	if (pageN == 0 && recordN == 0)
	{
		RenewpageHeader(fp, recordbuf, pageN); //���������� ���ڵ� ���� pazeN +1 = 0;
		readPage(fp, pagebuf, pageN);
		memcpy(pagebuf + HEADER_AREA_SIZE, recordbuf, length);
		writePage(fp, pagebuf, pageN);

		pageN=1;
		recordN++;
	}
	else if (deleteP == -1 && deleteR == -1) //�̶��� �̹� ���𰡰� �ְ� �����Ұ��� ������
	{
		readPage(fp, pagebuf, pageN-1); // ���� �������� �ִ� ���� �޾Ƶ��̰�
		
		if (ExistedLengthN(fp, pageN-1) + length > DATA_AREA_SIZE
			|| ExistedHeaderLengthN(fp, pageN-1) + 8 > HEADER_AREA_SIZE) // �������������ũ��)
		{
			RenewpageHeader(fp, recordbuf, pageN); //���������� ���ڵ� ����
			readPage(fp, pagebuf, pageN);
			 memcpy(pagebuf + HEADER_AREA_SIZE, recordbuf, length);
			writePage(fp, pagebuf, pageN);
			pageN++;
			recordN++;
		}
		else
		{ //���������� �ǵڿ� ���� 
			// ������ �ִ� ���ڵ带 �޾Ƶ鿩 pagebuf�� �����ϰ� ���ο� ���ڵ带 �ڿ� �ٿ��� �ٽ� ����
			readPage(fp, pagebuf, pageN-1);
			memcpy(pagebuf + ExistedLengthN(fp, pageN-1) + HEADER_AREA_SIZE, recordbuf, length);
			writePage(fp, pagebuf, pageN-1);
			RenewpageHeader(fp, recordbuf, pageN-1); //����
			recordN++;
		}
	} // 0 0 , -1 -1 �� �ƴ϶�� ��

	else
	{// ���� ������ �а� �ű��ִ� deleteR�� �ִ�
		//��� ���ٰ� 
		int beforeP, beforeR, NowP, NowR;
		
		char tmpPage[PAGE_SIZE];
		int One_Count = 0;


		readPage(fp, pagebuf, deleteP);

		//�ι�° ���� �� �տ��ִ°��� *�ڿ� �ִ°� �����ؾ���
		int tobeRestoredP, tobeRestoredR;
		int togiveP, togiveR;
		//Nowp Nowr�� �տ��ٰ� �������ְ� �ڱ� �ٲٸ� ��
		tobeRestoredP = deleteP;
		tobeRestoredR = deleteR;

		memcpy(&beforeP, pagebuf +HEADER_AREA_SIZE+ OffsetOfpr(fp, deleteP, deleteR) + 1, 4); // beforP ������. ����.
		memcpy(&beforeR, pagebuf + HEADER_AREA_SIZE + OffsetOfpr(fp, deleteP, deleteR) + 5, 4);

		if (length <= LengthOfpr(fp, deleteP, deleteR));
		{
			memcpy(pagebuf + HEADER_AREA_SIZE+OffsetOfpr(fp,deleteP,deleteR), recordbuf, length);
			writePage(fp, pagebuf, deleteP);

			deleteP = beforeP;
			deleteR = beforeR;
			One_Count = 1;
		}

		//
		//readPage(fp, pagebuf, deleteP);
		while (1)
		{
			//���� ��ġ : beforeP, breforeR�� ���� �ű⿡ ��
			//�װ����� ������, �װ��� �ִ� *���� deleteP,del
			if (One_Count == 1)break; // ó���� ã������ �Ѿ

			//beforeP, breforeR �� ���� �װ��� offset�� ����
			memset(pagebuf, (char)0xFF, MAX_RECORD_SIZE);
			readPage(fp, pagebuf, beforeP);
			if (length <= LengthOfpr(fp, beforeP, beforeR))
			{
				//���� �ű⿡
				char* ttmpPage[PAGE_SIZE];
				memset(ttmpPage, (char)0xFF, PAGE_SIZE);
				readPage(fp, ttmpPage, beforeP); //�ֱ�
				memcpy(ttmpPage + HEADER_AREA_SIZE + OffsetOfpr(fp, deleteP, deleteR), recordbuf, length);
				writePage(fp, ttmpPage, beforeP);
				//��� �� �� ������
				memcpy(&togiveP, pagebuf + OffsetOfpr(fp, beforeP, beforeR) + HEADER_AREA_SIZE + 1, 4);
				memcpy(&togiveR, pagebuf + OffsetOfpr(fp, beforeP, beforeR) + HEADER_AREA_SIZE + 5, 4);
				
				memset(tmpPage, (char)0xFF, sizeof(PAGE_SIZE)); //���� �� ���� tmp

				readPage(fp, tmpPage, tobeRestoredP); //�ű� ����

				memcpy(tmpPage + HEADER_AREA_SIZE + OffsetOfpr(fp,tobeRestoredP,tobeRestoredR)+1, &togiveP, 4); //����� ���� 
				memcpy(tmpPage + HEADER_AREA_SIZE + OffsetOfpr(fp, tobeRestoredP, tobeRestoredR)+5, &togiveR, 4);

				//������ ���� �ռ����� ����
				writePage(fp,tmpPage, tobeRestoredP);
				//������ �ֱ�
				break; //����
			}
			//�ٲܲ� : beforeP ���� ( ã�� �� , to be ResotredP ����. tobe = beforeP , beforeP = �ؾ����

			else //���־����� �����ؾ���
			{
				memset(tmpPage, (char)0xFF, sizeof(PAGE_SIZE)); //���� �� ���� tmp
				readPage(fp, tmpPage, beforeP); //�ű� ����
				int toBeforeP;
				int toBeforeR;
				memcpy(&toBeforeP,tmpPage + HEADER_AREA_SIZE + OffsetOfpr(fp, beforeP, beforeR) + 1, 4); //����� ���� 
				memcpy(&toBeforeR,tmpPage + HEADER_AREA_SIZE + OffsetOfpr(fp, beforeP, beforeR) + 5, 4);
				
				tobeRestoredP = beforeP;
				tobeRestoredR = beforeR;
				beforeP = toBeforeP;
				beforeR = toBeforeR;
			}


			if (beforeP == -1 || beforeR == -1) //������ ã���� ��
			{
				memset(pagebuf, (char)0xFF, PAGE_SIZE);
				readPage(fp, pagebuf, pageN - 1); // ���� �������� �ִ� ���� �޾Ƶ��̰�

				if (ExistedLengthN(fp, pageN - 1) + length > DATA_AREA_SIZE
					|| ExistedHeaderLengthN(fp, pageN - 1) + 8 > HEADER_AREA_SIZE) // �������������ũ��)
				{
					RenewpageHeader(fp, recordbuf, pageN); //���������� ���ڵ� ����
					readPage(fp, pagebuf, pageN);
					memcpy(pagebuf + HEADER_AREA_SIZE, recordbuf, length);
					writePage(fp, pagebuf, pageN);
					pageN++;
					recordN++;
				}
				else
				{ //���������� �ǵڿ� ���� 
					// ������ �ִ� ���ڵ带 �޾Ƶ鿩 pagebuf�� �����ϰ� ���ο� ���ڵ带 �ڿ� �ٿ��� �ٽ� ����
					readPage(fp, pagebuf, pageN - 1);
					memcpy(pagebuf + ExistedLengthN(fp, pageN - 1) + HEADER_AREA_SIZE, recordbuf, length);
					writePage(fp, pagebuf, pageN - 1);
					RenewpageHeader(fp, recordbuf, pageN - 1); //����
					recordN++;
				}
			break;
			} // 0 0 , -1 -1 �� �ƴ϶�� ��

		}
			
	}
	//������ڵ� ����

	memcpy(Headbuf, &pageN, 4);
	memcpy(Headbuf + 4, &recordN, 4);
	memcpy(Headbuf + 8, &deleteP, 4);
	memcpy(Headbuf + 12, &deleteR, 4);
	WriteHeaderRecord(fp, Headbuf); //�ִ³��� ���� fp��
}


//
// �ֹι�ȣ�� ��ġ�ϴ� ���ڵ带 ã�Ƽ� �����ϴ� ����� �����Ѵ�.
//

void delete(FILE* fp, const char* id)
{
	Person tmp;
	char headbuf[PAGE_SIZE], pagebuf[PAGE_SIZE],Headbuf[HEADER_AREA_SIZE];
	char recordbuf[MAX_RECORD_SIZE], readbuf[MAX_RECORD_SIZE], find[MAX_RECORD_SIZE];
	int i, j;
	int deleteOK = 0;
	int deleteP, deleteR, pageN, recordN;
	int length, PageRecordN, offset;

	int FindLength = strlen(id);

	memset(Headbuf, (char)0xFF, sizeof(Headbuf));
	memset(pagebuf, (char)0xFF, sizeof(pagebuf)); //���������� �ʱ�ȭ
	memset(recordbuf, (char)0xFF, sizeof(recordbuf)); //���ڵ� ����  �ʱ�ȭ
	memset(headbuf, (char)0xFF, sizeof(headbuf));
	memset(find, (char)0xFF, sizeof(find));



	ReadHeaderRecord(fp, Headbuf); //���ڵ� ���� // ���� ó��

	memcpy(&pageN, Headbuf, 4);
	memcpy(&recordN, Headbuf + 4, 4);
	memcpy(&deleteP, Headbuf + 8, 4);
	memcpy(&deleteR, Headbuf + 12, 4); //������ڵ� �б�

	char* str = (char*)malloc(FindLength);
	for (i = 1; i <= pageN; i++) 
	{
		
		int PageRecordN=0;
		readPage(fp, pagebuf, i-1);
		memcpy(&PageRecordN, pagebuf, 4);

		for (j = 0; j < PageRecordN; j++) //���ڵ����ŭ ã�� 
		{
			//i-1�������� j ���ڵ�
			memcpy(find, pagebuf + OffsetOfpr(fp,i-1,j) + HEADER_AREA_SIZE , 
			LengthOfpr(fp,i-1,j) ); //���� �� ������ 
			
			//printf("offset : %d , ���� : %d ", OffsetOfpr(fp, i - 1, j), LengthOfpr(fp, i - 1, j));
			
			
			memcpy(str, find, FindLength);

			if (!strcmp(str, id)) //������
			{
				
				memcpy(find + 1, &deleteP, 4);
				memcpy(find + 5, &deleteR, 4);
				deleteP = i-1; //������ ������ ��ȣ
				deleteR = j; //������ ���ڵ� ��ȣ
				deleteOK = 1;

				memcpy(pagebuf + OffsetOfpr(fp, i - 1, j), find, LengthOfpr(fp, i - 1, j));
				writePage(fp, pagebuf, i-1);
				break;

			}

		}
		if (deleteOK == 1)
			break;
		memset(pagebuf, (char)0xFF, sizeof(pagebuf));
		

	}
	memcpy(Headbuf + 8, &deleteP, 4);
	memcpy(Headbuf + 12, &deleteR, 4);

	WriteHeaderRecord(fp, Headbuf);
	free(str);
}


int main(int argc, char* argv[])
{

	FILE* fp;  // ���ڵ� ������ ���� ������
	char headbuf[PAGE_SIZE];
	char pagebuf[PAGE_SIZE];
	char readbuf[MAX_RECORD_SIZE];
	int deleteR = -1, deleteP = -1, pageN = 0, recordN = 0;
	Person person;

	memset(readbuf, (char)0xFF, sizeof(readbuf));
	memset(headbuf, (char)0xFF, sizeof(headbuf));
	memset(pagebuf, (char)0xFF, sizeof(pagebuf));
	memcpy(headbuf, &pageN, 4);

	memcpy(headbuf + 4, &recordN, 4);
	memcpy(headbuf + 8, &deleteR, 4);
	memcpy(headbuf + 12, &deleteP, 4);
	
	if ((fp = fopen(argv[2], "r+")) == 0)
	{ //�� ���
		fp = fopen(argv[2], "w+");//������ �׳� ����
		WriteHeaderRecord(fp, headbuf);
	}
	else 
	{
		fp = fopen(argv[2], "r+");
	}

	if (!strncmp(argv[1], "a", 1)) { 
		char* ptr;
		char S[50];// ũ�Ⱑ 30�� char�� �迭�� �����ϰ� ���ڿ� �Ҵ�
		strcpy(S, argv[3]);
		ptr = strtok(S, "\"");
		strcpy(person.id, ptr);
		strcpy(S, argv[4]);
		ptr = strtok(S, "\"");
		strcpy(person.name, ptr);
		strcpy(S, argv[5]);
		ptr = strtok(S, "\"");
		strcpy(person.age, ptr);
		strcpy(S, argv[6]);
		ptr = strtok(S, "\"");
		strcpy(person.addr, ptr);
		strcpy(S, argv[7]);
		ptr = strtok(S, "\"");
		strcpy(person.phone, ptr);
		strcpy(S, argv[8]);
		ptr = strtok(S, "\"");
		strcpy(person.email, ptr);


		add(fp, &person);
	}

	else if (!strncmp(argv[1], "d", 1)) 
	{
		char findid[14];
		char* ptr;
		strcpy(findid, argv[3]);
		ptr = strtok(findid, "\"");
		strcpy(findid, ptr);
		delete(fp, findid);
	}
	





	return 1;
}
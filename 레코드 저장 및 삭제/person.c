#define _CRT_SECURE_NO_WARNINGS
#define recordS  PAGE_SIZE/MAX_RECORD_SIZE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "person.h"
//필요한 경우 헤더 파일과 함수를 추가할 수 있음
  // 페이지사이즈 / 레코드 사이즈
// 과제 설명서대로 구현하는 방식은 각자 다를 수 있지만 약간의 제약을 둡니다.
// 레코드 파일이 페이지 단위로 저장 관리되기 때문에 사용자 프로그램에서 레코드 파일로부터 데이터를 읽고 쓸 때도
// 페이지 단위를 사용합니다. 따라서 아래의 두 함수가 필요합니다.
// 1. readPage(): 주어진 페이지 번호의 페이지 데이터를 프로그램 상으로 읽어와서 pagebuf에 저장한다
// 2. writePage(): 프로그램 상의 pagebuf의 데이터를 주어진 페이지 번호에 저장한다
// 레코드 파일에서 기존의 레코드를 읽거나 새로운 레코드를 쓰거나 삭제 레코드를 수정할 때나
// 위의 readPage() 함수를 호출하여 pagebuf에 저장한 후, 여기에 필요에 따라서 새로운 레코드를 저장하거나
// 삭제 레코드 관리를 위한 메타데이터를 저장합니다. 그리고 난 후 writePage() 함수를 호출하여 수정된 pagebuf를
// 레코드 파일에 저장합니다. 반드시 페이지 단위로 읽거나 써야 합니다.
//
// 주의: 데이터 페이지로부터 레코드(삭제 레코드 포함)를 읽거나 쓸 때 페이지 단위로 I/O를 처리해야 하지만,
// 헤더 레코드의 메타데이터를 저장하거나 수정하는 경우 페이지 단위로 처리하지 않고 직접 레코드 파일을 접근해서 처리한다.


void ReadHeaderRecord(FILE* fp, char* pagebuf)
{
	fseek(fp, 0, SEEK_SET); //처음에 나두고
	fread(pagebuf, 1, 16, fp); // 읽어드림
}
void WriteHeaderRecord(FILE* fp, char* pagebuf) //있는내용 쓰기 fp에
{
	fseek(fp, 0, SEEK_SET);
	fwrite(pagebuf, 1, 16, fp);
}

//
// 페이지 번호에 해당하는 페이지를 주어진 페이지 버퍼에 읽어서 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void readPage(FILE* fp, char* pagebuf, int pagenum) // 3번째있는걸 
{
	fseek(fp, 16, SEEK_SET);
	fseek(fp, pagenum * PAGE_SIZE, SEEK_CUR);
	fread(pagebuf, 1, PAGE_SIZE, fp);
}

//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 레코드 파일의 위치에 저장한다. 
// 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE* fp, const char* pagebuf, int pagenum)
{
	fseek(fp, 16, SEEK_SET);
	fseek(fp, pagenum * PAGE_SIZE, SEEK_CUR);
	fwrite(pagebuf, 1, PAGE_SIZE, fp);
}




//
// 새로운 레코드를 저장할 때 터미널로부터 입력받은 정보를 Person 구조체에 먼저 저장하고, pack() 함수를 사용하여
// 레코드 파일에 저장할 레코드 형태를 recordbuf에 만든다. 
// 
void pack(char* recordbuf, const Person* p)
{
	sprintf(recordbuf, "%s#%s#%s#%s#%s#%s#", p->id, p->name, p->age, p->addr, p->phone, p->email);
}

// 
// 아래의 unpack() 함수는 recordbuf에 저장되어 있는 레코드를 구조체로 변환할 때 사용한다.
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
//페이지에 존재하는 length 길이 리턴 
int ExistedLengthN(FILE* fp, int pagenum)
{
	char pagebuf[PAGE_SIZE];
	char PageHeadbuf[HEADER_AREA_SIZE];
	int result=0,tmp=0;
	int length2 = 0;
	int PageRecordN;
	readPage(fp, pagebuf, pagenum); // 페이지 읽고
	memcpy(&PageRecordN, pagebuf, 4);
	if (PageRecordN == -1|| PageRecordN == 0)
	{
		return 0;
	}
	for (int i = 0; i < PageRecordN; i++) // 만약 recordN가 -1이라면?
	{
		memcpy(&tmp, pagebuf+ (8*(i+1)), 4); //렝쓰 읽고 
		result += tmp; // 그것만큼 더함. 근데 주의점. 만약 삭제된거였다면?
	}
	return result;
}

//페이지에 존재하는 헤더 length 리턴
int ExistedHeaderLengthN(FILE* fp, int pagenum)
{
	char pagebuf[PAGE_SIZE];
	int result = 0;
	int PageRecordN=0;
	readPage(fp, pagebuf, pagenum); // 페이지 읽고

	memcpy(&PageRecordN, pagebuf, 4);
	if (PageRecordN == -1|| PageRecordN == 0)
	{
		return 0;
	}
	else
	result = 4 + 8 * (PageRecordN);

	return result;
}

int LengthOfpr(FILE* fp, int pagenum, int recordnum) //주어진 페이지와 레코드 넘버의 길이를 리턴
{ //0 포함생각
	char pagebuf[PAGE_SIZE];
	int result = 0;
	readPage(fp, pagebuf, pagenum); // 페이지 읽고
	memcpy(&result, pagebuf+8+recordnum*8, 4); //거기 있는 레코드의 length
	return result;
}

int OffsetOfpr(FILE* fp, int pagenum, int recordnum) //주어진 페이지와 레코드 넘버의 길이를 리턴
{
	char pagebuf[PAGE_SIZE];
	int result = 0;
	readPage(fp, pagebuf, pagenum); // 페이지 읽고
	memcpy(&result, pagebuf + 4 + recordnum * 8, 4); //거기 있는 레코드의 length
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
	readPage(fp, pagebuf, pagenum); //있을때로 가정하자
	memcpy(&recordN, pagebuf, 4); 
	if (recordN == -1|| recordN==0) //처음 넣을때
	{
		recordN = 1;
		memcpy(pagebuf+4, &offset, 4); //여기 뒤부터 8바이트 적으면됨 oofset , length
		memcpy(pagebuf+ 8, &length, 4);//적음 아 페이지단위로해야함
		memcpy(pagebuf, &recordN, 4);
		writePage(fp, pagebuf, pagenum);
	}
	else
	{
		offset = HEADER_AREA_SIZE+OffsetOfpr(fp, pagenum, recordN-1) + LengthOfpr(fp, pagenum, recordN-1); // 이전거 + length = 현재 offset
		recordN++;
		memcpy(pagebuf + ExistedHeaderLengthN(fp, pagenum), &offset, 4); //여기 뒤부터 8바이트 적으면됨 oofset , length
		memcpy(pagebuf + ExistedHeaderLengthN(fp, pagenum) +4, &length, 4);//적음 아 페이지단위로해야함
		memcpy(pagebuf, &recordN, 4);

		writePage(fp, pagebuf, pagenum);
	}
	
}

//
// 새로운 레코드를 저장하는 기능을 수행하며, 터미널로부터 입력받은 필드값들을 구조체에 저장한 후 아래 함수를 호출한다.
//
void add(FILE* fp, const Person* p)
{

	char recordbuf[MAX_RECORD_SIZE];
	char pagebuf[PAGE_SIZE];
	char Headbuf[16], PageHeadbuf[HEADER_AREA_SIZE]; // 맨처음, 페이지마다
	int pageN;
	int recordN;
	int length, PageRecordN, offset;
	int deleteP, deleteR;

	memset(Headbuf, (char)0xFF, sizeof(Headbuf));
	memset(pagebuf, (char)0xFF, sizeof(pagebuf)); //페이지버퍼 초기화
	memset(recordbuf, (char)0xFF, sizeof(recordbuf)); //레코드 버퍼  초기화

	pack( recordbuf, p ); //구조체에 저장하기 , 레코드버퍼에 저장 // 길이 알기
	length = strlen(recordbuf);
	

	ReadHeaderRecord(fp, Headbuf); //레코드 갱신 // 만약 처음이면  0 0 -1 -1 거든?
	

	memcpy(&pageN, Headbuf, 4); //페이지 수 (이거 다름 원래는 페이지 수 앞에 +1이었음_)
	memcpy(&recordN, Headbuf + 4, 4); //레코드수 (헤더포함)
	memcpy(&deleteP, Headbuf + 8, 4); //삭제할때 페이지
	memcpy(&deleteR, Headbuf + 12, 4);// 삭제할때 레코드


	//pageN이랑 recordN 조정해야함.  메인에서 0 0 -1-1로 주어야함
	//offset이랑 length랑 PageRecordN도 조정해야함.!
	if (pageN == 0 && recordN == 0)
	{
		RenewpageHeader(fp, recordbuf, pageN); //다음페이지 레코드 저장 pazeN +1 = 0;
		readPage(fp, pagebuf, pageN);
		memcpy(pagebuf + HEADER_AREA_SIZE, recordbuf, length);
		writePage(fp, pagebuf, pageN);

		pageN=1;
		recordN++;
	}
	else if (deleteP == -1 && deleteR == -1) //이때는 이미 무언가가 있고 삭제할곳이 없을때
	{
		readPage(fp, pagebuf, pageN-1); // 현재 페이지에 있는 내용 받아들이고
		
		if (ExistedLengthN(fp, pageN-1) + length > DATA_AREA_SIZE
			|| ExistedHeaderLengthN(fp, pageN-1) + 8 > HEADER_AREA_SIZE) // 페이지사이즈보다크면)
		{
			RenewpageHeader(fp, recordbuf, pageN); //다음페이지 레코드 저장
			readPage(fp, pagebuf, pageN);
			 memcpy(pagebuf + HEADER_AREA_SIZE, recordbuf, length);
			writePage(fp, pagebuf, pageN);
			pageN++;
			recordN++;
		}
		else
		{ //현재페이지 맨뒤에 적기 
			// 기존에 있던 레코드를 받아들여 pagebuf에 저장하고 새로운 레코드를 뒤에 붙여서 다시 저장
			readPage(fp, pagebuf, pageN-1);
			memcpy(pagebuf + ExistedLengthN(fp, pageN-1) + HEADER_AREA_SIZE, recordbuf, length);
			writePage(fp, pagebuf, pageN-1);
			RenewpageHeader(fp, recordbuf, pageN-1); //갱신
			recordN++;
		}
	} // 0 0 , -1 -1 다 아니라면 삭

	else
	{// 삭제 페이지 읽고 거기있는 deleteR에 있는
		//계속 돌다가 
		int beforeP, beforeR, NowP, NowR;
		
		char tmpPage[PAGE_SIZE];
		int One_Count = 0;


		readPage(fp, pagebuf, deleteP);

		//두번째 돌면 이 앞에있는것의 *뒤에 있는걸 저장해야함
		int tobeRestoredP, tobeRestoredR;
		int togiveP, togiveR;
		//Nowp Nowr을 앞에다가 삽입해주고 자기 바꾸면 끝
		tobeRestoredP = deleteP;
		tobeRestoredR = deleteR;

		memcpy(&beforeP, pagebuf +HEADER_AREA_SIZE+ OffsetOfpr(fp, deleteP, deleteR) + 1, 4); // beforP 저장중. 갈곳.
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
			//현재 위치 : beforeP, breforeR에 있음 거기에 지
			//그곳으로 간다음, 그곳에 있는 *값을 deleteP,del
			if (One_Count == 1)break; // 처음에 찾았으면 넘어감

			//beforeP, breforeR 로 가서 그곳의 offset을 읽음
			memset(pagebuf, (char)0xFF, MAX_RECORD_SIZE);
			readPage(fp, pagebuf, beforeP);
			if (length <= LengthOfpr(fp, beforeP, beforeR))
			{
				//넣자 거기에
				char* ttmpPage[PAGE_SIZE];
				memset(ttmpPage, (char)0xFF, PAGE_SIZE);
				readPage(fp, ttmpPage, beforeP); //넣기
				memcpy(ttmpPage + HEADER_AREA_SIZE + OffsetOfpr(fp, deleteP, deleteR), recordbuf, length);
				writePage(fp, ttmpPage, beforeP);
				//들어 갈 수 있으면
				memcpy(&togiveP, pagebuf + OffsetOfpr(fp, beforeP, beforeR) + HEADER_AREA_SIZE + 1, 4);
				memcpy(&togiveR, pagebuf + OffsetOfpr(fp, beforeP, beforeR) + HEADER_AREA_SIZE + 5, 4);
				
				memset(tmpPage, (char)0xFF, sizeof(PAGE_SIZE)); //넣을 곳 버퍼 tmp

				readPage(fp, tmpPage, tobeRestoredP); //거기 저장

				memcpy(tmpPage + HEADER_AREA_SIZE + OffsetOfpr(fp,tobeRestoredP,tobeRestoredR)+1, &togiveP, 4); //저장될 곳에 
				memcpy(tmpPage + HEADER_AREA_SIZE + OffsetOfpr(fp, tobeRestoredP, tobeRestoredR)+5, &togiveR, 4);

				//넣을거 저장 앞선곳에 저장
				writePage(fp,tmpPage, tobeRestoredP);
				//실제로 넣기
				break; //갱신
			}
			//바꿀꺼 : beforeP 갱신 ( 찾을 곳 , to be ResotredP 갱신. tobe = beforeP , beforeP = 잊어버림

			else //못넣었으면 갱신해야함
			{
				memset(tmpPage, (char)0xFF, sizeof(PAGE_SIZE)); //넣을 곳 버퍼 tmp
				readPage(fp, tmpPage, beforeP); //거기 저장
				int toBeforeP;
				int toBeforeR;
				memcpy(&toBeforeP,tmpPage + HEADER_AREA_SIZE + OffsetOfpr(fp, beforeP, beforeR) + 1, 4); //저장될 곳에 
				memcpy(&toBeforeR,tmpPage + HEADER_AREA_SIZE + OffsetOfpr(fp, beforeP, beforeR) + 5, 4);
				
				tobeRestoredP = beforeP;
				tobeRestoredR = beforeR;
				beforeP = toBeforeP;
				beforeR = toBeforeR;
			}


			if (beforeP == -1 || beforeR == -1) //끝까지 찾았을 때
			{
				memset(pagebuf, (char)0xFF, PAGE_SIZE);
				readPage(fp, pagebuf, pageN - 1); // 현재 페이지에 있는 내용 받아들이고

				if (ExistedLengthN(fp, pageN - 1) + length > DATA_AREA_SIZE
					|| ExistedHeaderLengthN(fp, pageN - 1) + 8 > HEADER_AREA_SIZE) // 페이지사이즈보다크면)
				{
					RenewpageHeader(fp, recordbuf, pageN); //다음페이지 레코드 저장
					readPage(fp, pagebuf, pageN);
					memcpy(pagebuf + HEADER_AREA_SIZE, recordbuf, length);
					writePage(fp, pagebuf, pageN);
					pageN++;
					recordN++;
				}
				else
				{ //현재페이지 맨뒤에 적기 
					// 기존에 있던 레코드를 받아들여 pagebuf에 저장하고 새로운 레코드를 뒤에 붙여서 다시 저장
					readPage(fp, pagebuf, pageN - 1);
					memcpy(pagebuf + ExistedLengthN(fp, pageN - 1) + HEADER_AREA_SIZE, recordbuf, length);
					writePage(fp, pagebuf, pageN - 1);
					RenewpageHeader(fp, recordbuf, pageN - 1); //갱신
					recordN++;
				}
			break;
			} // 0 0 , -1 -1 다 아니라면 삭

		}
			
	}
	//헤더레코드 변경

	memcpy(Headbuf, &pageN, 4);
	memcpy(Headbuf + 4, &recordN, 4);
	memcpy(Headbuf + 8, &deleteP, 4);
	memcpy(Headbuf + 12, &deleteR, 4);
	WriteHeaderRecord(fp, Headbuf); //있는내용 쓰기 fp에
}


//
// 주민번호와 일치하는 레코드를 찾아서 삭제하는 기능을 수행한다.
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
	memset(pagebuf, (char)0xFF, sizeof(pagebuf)); //페이지버퍼 초기화
	memset(recordbuf, (char)0xFF, sizeof(recordbuf)); //레코드 버퍼  초기화
	memset(headbuf, (char)0xFF, sizeof(headbuf));
	memset(find, (char)0xFF, sizeof(find));



	ReadHeaderRecord(fp, Headbuf); //레코드 갱신 // 만약 처음

	memcpy(&pageN, Headbuf, 4);
	memcpy(&recordN, Headbuf + 4, 4);
	memcpy(&deleteP, Headbuf + 8, 4);
	memcpy(&deleteR, Headbuf + 12, 4); //헤더레코드 읽기

	char* str = (char*)malloc(FindLength);
	for (i = 1; i <= pageN; i++) 
	{
		
		int PageRecordN=0;
		readPage(fp, pagebuf, i-1);
		memcpy(&PageRecordN, pagebuf, 4);

		for (j = 0; j < PageRecordN; j++) //레코드수만큼 찾음 
		{
			//i-1페이지에 j 레코드
			memcpy(find, pagebuf + OffsetOfpr(fp,i-1,j) + HEADER_AREA_SIZE , 
			LengthOfpr(fp,i-1,j) ); //읽을 곳 페이지 
			
			//printf("offset : %d , 길이 : %d ", OffsetOfpr(fp, i - 1, j), LengthOfpr(fp, i - 1, j));
			
			
			memcpy(str, find, FindLength);

			if (!strcmp(str, id)) //같으면
			{
				
				memcpy(find + 1, &deleteP, 4);
				memcpy(find + 5, &deleteR, 4);
				deleteP = i-1; //삭제한 페이지 번호
				deleteR = j; //삭제한 레코드 번호
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

	FILE* fp;  // 레코드 파일의 파일 포인터
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
	{ //다 덮어씀
		fp = fopen(argv[2], "w+");//없으면 그냥 생성
		WriteHeaderRecord(fp, headbuf);
	}
	else 
	{
		fp = fopen(argv[2], "r+");
	}

	if (!strncmp(argv[1], "a", 1)) { 
		char* ptr;
		char S[50];// 크기가 30인 char형 배열을 선언하고 문자열 할당
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
// 주의사항
// 1. blockmap.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. blockmap.h에 정의되어 있지 않을 경우 본인이 이 파일에서 만들어서 사용하면 됨
// 3. 필요한 data structure가 필요하면 이 파일에서 정의해서 쓰기 바람(blockmap.h에 추가하면 안됨)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "blockmap.h"
// 필요한 경우 헤더 파일을 추가하시오.

int dd_read(int , char*);
int dd_write(int , char*);
int dd_erase(int);
//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// file system에 의해 반드시 먼저 호출이 되어야 한다.
//
FILE* flashfp;
struct Address_mapping
{
	int lbn;
	int pbn;
};
typedef struct Address_mapping Add_map;
Add_map Add_map_table[BLOCKS_PER_DEVICE];

int Free_Block_Pbn;
void ftl_open()
{
	for(int i=0;i<DATABLKS_PER_DEVICE;i++)
	{
		Add_map_table[i].lbn=i;
		Add_map_table[i].pbn=-1;
	}
	if(access ("flashmemory",F_OK) !=-1 ) //각 블록의 첫페이지 읽어서 매피ㅇ테이블복구
	{
		
		int tmp;
		char* pagebuf;
		pagebuf=(char*)malloc(sizeof(char)*PAGE_SIZE);
		memset(pagebuf,0xFF,PAGE_SIZE);
	
		for(int i=0;i<BLOCKS_PER_DEVICE;i++) // loading lbn info from spare
		{
			dd_read(i*PAGES_PER_BLOCK,pagebuf); //read page buffer has lbn
			memcpy(&tmp,pagebuf+SECTOR_SIZE,sizeof(int)); //use lbn to pbn
	//		printf("i : %d  pbn : %d\n",i,tmp); 
		//good


			for(int j=0;j<DATABLKS_PER_DEVICE;j++)
			{
				if(Add_map_table[j].lbn==tmp)
				{
					Add_map_table[tmp].pbn=i;
					break; 
				}
			/*
		   	   if(j == DATABLKS_PER_DEVICE -1) //할당 안된애들 다 -1
				{
					if(Add_map_table[i].Isputted ==1) // if putted
					break;
					printf("\n%d 번째 블럭 pbn : -1됨.\n",i);
					
					Add_map_table[].pbn=-1;
				}
			*/
			}	



		}
		
		



 //right
		free(pagebuf);
		int Is_there[BLOCKS_PER_DEVICE];
	
		memset(Is_there,0,BLOCKS_PER_DEVICE);
	
		for(int i=0;i<DATABLKS_PER_DEVICE;i++)
		{
			
			if(Add_map_table[i].pbn == -1)
				continue;

			else
				Is_there[Add_map_table[i].pbn]=1; // put true in table[pbn]

		}	
		for(int i=0;i<BLOCKS_PER_DEVICE;i++) //block a
		{
			if(Is_there[i]==0) // false doesnt't exist in pbn
			{	
				Free_Block_Pbn = i;
				break;
			}
		}
	}
	else //when flash_memory doesn't exist
	{
	for(int i=0;i<DATABLKS_PER_DEVICE;i++) // initiate
		Add_map_table[i].pbn=-1;
		Free_Block_Pbn=0;
	}

	// free block's pbn 초기화


	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_read(int lsn, char *sectorbuf)
{
	//lsn 에 대응되는  p피엔 구하고  전달
	char* pagebuffer = (char*)malloc(sizeof(char)*PAGE_SIZE);
	int tmppbn,tmpppn,lbn,offset;
	lbn=lsn/PAGES_PER_BLOCK;
	offset=lsn%PAGES_PER_BLOCK;

	tmppbn=Add_map_table[lbn].pbn;
	tmpppn=tmppbn*PAGES_PER_BLOCK + offset;
	
	dd_read(tmpppn,pagebuffer); //
	memcpy(sectorbuf,pagebuffer,SECTOR_SIZE); // isn't SECTOR_SIZE?
	free(pagebuffer);
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_write(int lsn, char *sectorbuf)
{
	int tmplbn; 
	int offset,lpn,ppn,pbn;
	int lbnNum=lsn/PAGES_PER_BLOCK;
	offset=lsn%PAGES_PER_BLOCK;
	char* pagebuffer=(char*)malloc(sizeof(char)*PAGE_SIZE);
	
	memset(pagebuffer, 0xFF, PAGE_SIZE);
	memcpy(pagebuffer,sectorbuf,SECTOR_SIZE);
	memcpy(pagebuffer+SECTOR_SIZE,&lbnNum,sizeof(int));
	memcpy(pagebuffer+SECTOR_SIZE+sizeof(int),&lsn,sizeof(int));  //put lbn, lsn to pagebuffer

	pbn=lbnNum+1; // setting pbn
	ppn=PAGES_PER_BLOCK * pbn + offset;

	if(Add_map_table[lbnNum].pbn== -1) // when there is no data ia pbn (first time)
	{
		char* writelbn=(char*)malloc(sizeof(char)*sizeof(int)); //lsn make
		memset(writelbn, 0xFF, sizeof(int));
	
		memcpy(writelbn,&lbnNum,sizeof(int)); //write lsn in first page in pbn
		
		if(offset==0) // when it is first page
			dd_write(ppn,pagebuffer);

		else	
		{
		dd_write(pbn*PAGES_PER_BLOCK+SECTOR_SIZE,writelbn); //put lbn
		dd_write(ppn,pagebuffer);//lbn and lsn put
		}
		free(writelbn);
		Add_map_table[lbnNum].pbn=pbn;
	}	
	else    //exist in pbn
	{
		pbn=Add_map_table[lbnNum].pbn;
		ppn=pbn*4+offset;
		char* tmpbuffer=(char*)malloc(sizeof(char)*PAGE_SIZE);
		int Is_Isn;
		dd_read(ppn,tmpbuffer); //read tmpbuffer
		memcpy(&Is_Isn,tmpbuffer+SECTOR_SIZE+4,4); //read isn
		free(tmpbuffer);
		if(Is_Isn<0 ) //doesn't exist in page but block //lsn 
		{
			dd_write(ppn,pagebuffer);
		}
		else                 //if it exist and overwrited
		{
			char* tmp = (char*)malloc(sizeof(char)*PAGE_SIZE);
			for(int i=0;i<PAGES_PER_BLOCK;i++) //overwrite first
			{
				if(i==offset) continue;
				dd_read(pbn*PAGES_PER_BLOCK+i,tmp); //overwrite
				dd_write(Free_Block_Pbn*PAGES_PER_BLOCK+i,tmp);
			}
			 //when offset
			dd_write(Free_Block_Pbn*PAGES_PER_BLOCK + offset,pagebuffer);
			free(tmp);
	
			dd_erase(pbn);
			int ttmp=Free_Block_Pbn;
			Free_Block_Pbn=pbn;
			Add_map_table[lbnNum].pbn=ttmp;

		}
	}

	free(pagebuffer);
	return;
}

void ftl_print()
{
	for(int i=0;i<DATABLKS_PER_DEVICE;i++)
	printf("%d %d\n",Add_map_table[i].lbn,Add_map_table[i].pbn);
	return;
}

// 
// 과제3의 채점 프로그램은 기본적으로 아래와 같이 동작함
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "blockmapping.h"

FILE *flashmemoryfp;


/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void ftl_print();

//
// 이 함수는 file system의 역할을 수행한다고 생각하면 되고,
// file system이 flash memory로부터 512B씩 데이터를 저장하거나 데이터를 읽어 오기 위해서는
// 각자 구현한 FTL의 ftl_write()와 ftl_read()를 호출하면 됨
//
int main(int argc, char *argv[])
{
	char *blockbuf;
    char sectorbuf[SECTOR_SIZE];
	int lsn, i;


    flashmemoryfp = fopen("flashmemory", "w+b");
	if(flashmemoryfp == NULL)
	{
		printf("file open error\n");
		exit(1);
	}
	   
    //
    // flash memory의 모든 바이트를 '0xff'로 초기화한다.
    // 
    blockbuf = (char *)malloc(BLOCK_SIZE);
	memset(blockbuf, 0xFF, BLOCK_SIZE);

	for(i = 0; i < BLOCKS_PER_DEVICE; i++)
	{
		fwrite(blockbuf, BLOCK_SIZE, 1, flashmemoryfp);
	}

	free(blockbuf);

	ftl_open();    // ftl_read(), ftl_write() 호출하기 전에 이 함수를 반드시 호출해야 함

	//
	// ftl_write() 및 ftl_read() 테스트 코드 작성
	//

    ftl_write(10, "FIRST WRITE()");
 //   ftl_read(10, sectorbuf);
    ftl_write(10, "SECOND TRY");
//    ftl_read(10, sectorbuf);
//    ftl_read(10, sectorbuf);
//    ftl_read(10, sectorbuf);
 //   ftl_write(2, "Test 1 ");
 //   ftl_write(2, "Test 1 ");
//    ftl_read(2, sectorbuf);
//    ftl_read(30, sectorbuf);
//    ftl_read(10, sectorbuf);
//    ftl_write(3, "Dfdfa");
//    ftl_write(0, "Dfafas");
//    ftl_write(3, "Dfdfa");
    ftl_write(20, "Test 54541");
//
//    ftl_read(20, sectorbuf);
//    ftl_write(13, "Test 1");
//    ftl_write(14, "Test 1");
//    ftl_write(16, "Test 1");
   ftl_write(20, "CALL ME BABYYYYY");
//    ftl_read(20, sectorbuf);
//    ftl_read(20, sectorbuf);
//    ftl_write(20, "TEST 3");
//    ftl_read(20, sectorbuf);
//    ftl_write(30,"TEST 2 ");
//    ftl_read(30, sectorbuf);
//    ftl_write(50, "TETE");
//    ftl_read(50, sectorbuf);
//    ftl_write(50, "TETE");
//    ftl_write(59, "dfd");
//    ftl_read(59, sectorbuf);
//    ftl_write(59, "dfd");
//    ftl_write(3, "dfd");
//    ftl_write(3, "dfd");
//    ftl_write(5, "dfafasdf");
//    ftl_write(5, "dfafasdf");
//    ftl_write(5, "dfafasdf");
//    ftl_write(9, "dfafasdf");
//    ftl_write(9, "dfafasdf");
//    ftl_write(12, "dfafasdf");
//    ftl_write(16, "dfafasdf");
//    ftl_write(20, "TEST 1");
//    ftl_write(24, "TEST 1");
//    ftl_write(32, "TEST 1");
//    ftl_write(36, "TEST 1");
//    ftl_write(40, "TEST 1");
//    ftl_write(44, "TEST 1");
//    ftl_write(52, "TEST 1");
//
//    ftl_write(0, "TEST 1");
//    ftl_write(0, "ABCDEFGHIJK");
//
//    ftl_write(44, "dfafasfas");
//    ftl_write(45, "dfafsf");
//    ftl_write(45, "dfafsf");
 //   ftl_read(5, sectorbuf);

   ftl_print();


	fclose(flashmemoryfp);

	return 0;
}

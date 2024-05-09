// 주의사항
// 1. blockmapping.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. blockmapping.h에 정의되어 있지 않을 경우 본인이 이 파일에서 만들어서 사용하면 됨
// 3. 필요한 data structure가 필요하면 이 파일에서 정의해서 쓰기 바람(blockmapping.h에 추가하면 안됨)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "blockmapping.h"
// 필요한 경우 헤더 파일을 추가하시오.

//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// file system에 의해 반드시 먼저 호출이 되어야 한다.
//

int dd_read(int ppn, char *pagebuf);
int dd_write(int ppn, char *pagebuf);
int dd_erase(int pbn);

int address_mapping_table[DATABLKS_PER_DEVICE];
int free_block = 0;
int block_indx = 1;

void ftl_open()
{
	//
	// address mapping table 초기화 또는 복구
	// free block's pbn 초기화
    // address mapping table에서 lbn 수는 DATABLKS_PER_DEVICE 동일

    //address mapping table 값을 전부 -1 로 초기화
    memset(address_mapping_table, -1, sizeof(address_mapping_table));

    //초기 free block pbn = 0.
    free_block=0;

	return;
}


//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_read(int lsn, char *sectorbuf)
{
    int lbn = lsn / PAGES_PER_BLOCK;
    int pbn = address_mapping_table[lbn];
    int offset = lsn % PAGES_PER_BLOCK;

    char pagebuf[PAGE_SIZE];
    if(pbn != -1) {
        if (dd_read(pbn * PAGES_PER_BLOCK + offset, pagebuf) == 1) {
        //printf("READ() PAGEBUF : %s\n", pagebuf);
            memcpy(sectorbuf, pagebuf, SECTOR_SIZE);
//        printf("READ() ppn %d SECTOR DATA : \n", pbn * PAGES_PER_BLOCK + offset );

//        for(int i=0; i<SECTOR_SIZE; i++){
//            printf("%02x ", sectorbuf[i]);
//        }
//        printf("\n");

//            char sparebuf[SPARE_SIZE];
//            memcpy(sparebuf, pagebuf + SECTOR_SIZE, SPARE_SIZE);
//            char first_four_bytes[SPARE_SIZE / 4];
//            memcpy(first_four_bytes, sparebuf, SPARE_SIZE / 4);
//        printf("READ() ppn %d SPARE DATA : \n", pbn * PAGES_PER_BLOCK + offset);

//            for(int i=0; i<SPARE_SIZE; i++){
//                printf("%02x ", sparebuf[i]);
//            }
//            printf("\n");

        }
        else {
            fprintf(stderr, "flash memory read error\n");
            exit(1);
        }
    }
    else{
        fprintf(stderr, "Error : matching pbn is -1\n");
        exit(1);
    }

	return;
}


//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_write(int lsn, char *sectorbuf)
{
    if(lsn<0 || lsn >59){
        fprintf(stderr, "Error : Input lsn %d is out of range\n", lsn);
        exit(1);
    }

    int lbn = lsn / PAGES_PER_BLOCK;
    int pbn = address_mapping_table[lbn];
    int offset = lsn % PAGES_PER_BLOCK;

    char pagebuf[PAGE_SIZE];
    memset(pagebuf, 0xff, PAGE_SIZE);

    //pagebuf에 data 적는 과정..
    //pagebuf의 sector area에  sectorbuf 내용 쓰기
    memcpy(pagebuf, sectorbuf, strlen(sectorbuf));
//    printf("WRITE() SECTOR DATA : %s\n", pagebuf);

    char lsnbuf[SPARE_SIZE/4];
    sprintf(lsnbuf,  "%d", lsn);
//    printf("WRITE() SPARE DATA : %s\n", lsnbuf);

    //pagebuf의 spare area에  lsn 쓰기
    memcpy(pagebuf+SECTOR_SIZE, lsnbuf, strlen(lsnbuf));


    //lsn에 최초로 데이터를 쓰는 경우
    if( pbn == -1 ){

        //block_indx번째 블록으로 이동 후 write.
        dd_write(block_indx * PAGES_PER_BLOCK + offset, pagebuf);

        //address mapping table update.
        address_mapping_table[lbn] = block_indx++;

    }
    // 해당 블럭에 -1이 아닌 값이 적혀있는 경우..
    // pbn의 offset이 비어있어서 가서 바로 적을 수 있는 경우와 free block으로 copy 떠줘야하는 경우로 나뉨..
    //spare area에 값이 적혀있는지 아닌지로 나누어줌..
    else{
        char pagebuf_tmp[PAGE_SIZE];

        char all_ff[SPARE_SIZE/4];
        memset(all_ff, 0xff, SPARE_SIZE/4);

        dd_read(pbn*PAGES_PER_BLOCK+offset, pagebuf_tmp);


        char first_four_bytes[SPARE_SIZE/4];
        memcpy(first_four_bytes, pagebuf_tmp+SECTOR_SIZE, SPARE_SIZE/4);


        //offset 위치가 비어있는 경우에는 그냥 쓰면 되고
        if(memcmp(first_four_bytes, all_ff , SPARE_SIZE/4) == 0 ){

            dd_write(pbn*PAGES_PER_BLOCK+offset, pagebuf);

        }
        //offset 위치에 이미 적혀있는 경우에는 free block에 옮겨주고 offset에 적는다.
        else{
            //free block에 copy.
            for(int i=0; i<4; i++) {
                if (i != offset) {
                    char lsn_area_tmp[SPARE_SIZE/4];
                    dd_read(pbn * PAGES_PER_BLOCK + i, pagebuf_tmp);
                    memcpy(lsn_area_tmp, pagebuf_tmp+SECTOR_SIZE, SPARE_SIZE/4);
                    if(memcmp(lsn_area_tmp, all_ff , SPARE_SIZE/4) != 0 ) {
                        //printf("%d\n", *spare_area);
                        dd_write(free_block * PAGES_PER_BLOCK + i, pagebuf_tmp);
                    }
                }
            }

            dd_write(free_block*PAGES_PER_BLOCK + offset, pagebuf);

            //기존 pbn erase.
            dd_erase(pbn);

            //free_block 변경 및 address mapping table update.
            address_mapping_table[lbn] = free_block;
            free_block = pbn;

        }

    }

    return;
}

void ftl_print()
{
    printf("lbn pbn\n");
    for(int i = 0; i < DATABLKS_PER_DEVICE; i++) {
        printf("%d %d\n",i, address_mapping_table[i]);
    }

    //free block number 출력...
    printf("free block=%d\n",free_block);

    //block_indx test
   // printf("block_indx=%d\n", block_indx);

	return;
}
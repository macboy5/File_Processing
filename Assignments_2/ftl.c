#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "flash.h"
// 필요한 경우 헤더파일을 추가한다

FILE *flashfp;	// fdevicedriver.c에서 사용

//
// 이 함수는 FTL의 역할 중 일부분을 수행하는데 물리적인 저장장치 flash memory에 Flash device driver를 이용하여 데이터를
// 읽고 쓰거나 블록을 소거하는 일을 한다.
// flash memory에 데이터를 읽고 쓰거나 소거하기 위해서 fdevicedriver.c에서 제공하는 인터페이스를
// 호출하면 된다. 이때 해당되는 인터페이스를 호출할 때 연산의 단위를 정확히 사용해야 한다.
// 참고로, 읽기와 쓰기는 페이지 단위이며 소거는 블록 단위이다.
//

extern int dd_read(int ppn, char *pagebuf);
extern int dd_write(int ppn, char *pagebuf);
extern int dd_erase(int pbn);

int pbn;


void flash_memory_emulator(char* filename, int pbn){

    flashfp = fopen( filename, "w+");

    for(int i=0; i<pbn; i++){
        dd_erase(i);

    }
    fclose(flashfp);

/*
    flashfp = fopen(filename, "r");

    fseek(flashfp, 15*PAGE_SIZE, SEEK_SET);
    int first_byte = fgetc(flashfp);
    printf("%02X", first_byte);
    fclose(flashfp);
*/


}

void page_write(char* filename, int ppn, char* sectordata, char* sparedata){

    //pbn = 0인 블록에 write하면 안되므로 exception 처리
    if(ppn <PAGE_NUM){
        fprintf(stderr, "0th block error()");
        exit(1);
    }



    int page_read_cnt = 0, page_write_cnt = 0, block_erase_cnt=0;

    flashfp = fopen( filename, "r+");
    fseek(flashfp, ppn*PAGE_SIZE, SEEK_SET);

    char pagebuf[PAGE_SIZE];

/*    int first_byte_of_page = fgetc(flashfp);
    printf("%02X", first_byte_of_page);*/
/*
    page_read_cnt++;
*/

    //페이지의 첫번째 바이트를 읽어서 그게 0xFF이면 empty로 판단.
    if ( fgetc(flashfp) != 0xFF){

        //이미 기존 데이터가 존재하는 경우.

        int order = ppn%PAGE_NUM; // 0 1 2 3 순

        //ppn이 있는 블록의 첫번째 페이지로 포인터 이동.
        fseek(flashfp, (ppn/PAGE_NUM)*PAGE_NUM * PAGE_SIZE, SEEK_SET);

        //pbn = 0 인 블록에 데이터 copy 작업 수행.
        //1.copy()
        page_read_cnt = 4;
        for(int i=0; i<4; i++){
            if(i != order){
                dd_read(ppn-order +i, pagebuf);

                fseek(flashfp, -PAGE_SIZE,SEEK_CUR);
                if(fgetc(flashfp) != 0xFF){ //데이터가 들어있을 때만 copy작업수행.
                    dd_write(i, pagebuf);

                    page_write_cnt++;
                }
            }

        }

        //block 단위로 기존 데이터 삭제.
        //2. erase()
        dd_erase(ppn/PAGE_NUM);

        block_erase_cnt++;


        fseek(flashfp, 0, SEEK_SET);

        //3. re-copy 작업
        for(int i=0; i<4; i++){
            fseek(flashfp, PAGE_SIZE*i, SEEK_SET);
            if(fgetc(flashfp) != 0xFF){ //데이터가 들어있을 때만 copy작업수행.
                dd_read(i,pagebuf);
                page_read_cnt++;

                dd_write( ppn-order+i, pagebuf);

                page_write_cnt++;
            }
        }

        //4.erase()

        if(page_write_cnt>0) {
            dd_erase(0);
            block_erase_cnt++;
        }

        //이제 쓰려는 위치가 비어있는 상태가 되었으므로

        //전체 페이지 0xFF로 채우기
        memset((void*)pagebuf, (char)0xFF, PAGE_SIZE);

        //sectordata 삽입
        memcpy((void*)pagebuf, sectordata, strlen(sectordata));

        //sparedata 삽입
        memcpy((void*)pagebuf + SECTOR_SIZE, sparedata, strlen(sparedata));

        dd_write(ppn, pagebuf);
        page_write_cnt++;
    }
    else{
        //애초에 비어 있는 페이지일 경우.
        page_read_cnt++;

        //전체 페이지 0xFF로 채우기
        memset((void*)pagebuf, (char)0xFF, PAGE_SIZE);

        //sectordata 삽입
        memcpy((void*)pagebuf, sectordata, strlen(sectordata));

        //sparedata 삽입
        memcpy((void*)pagebuf+ SECTOR_SIZE, sparedata, strlen(sparedata));

        dd_write(ppn, pagebuf);
        page_write_cnt++;
    }

    fclose(flashfp);


    //화면에 총페이지 읽기수 ,총페이지 쓰기수, 총블록소거수를 출력해야함.
    printf("#pageread = %d ", page_read_cnt);
    printf("#pagewrites = %d ", page_write_cnt);
    printf("#blockerases =  %d ", block_erase_cnt);
}

void page_read(char* filename, int ppn){
    //ppn에 저장되어있는 sectordata와 sparedata 화면에 출력
    //0xFF 이전 데이터까지만 출력.

    flashfp = fopen( filename, "r+");

    //일단 페이지 다 읽고...sector부분 spare부분 출력.
    char pagebuf[PAGE_SIZE];
    dd_read(ppn, pagebuf);

    int tmp=0;
    for(int i=0; i<SECTOR_SIZE; i++){
        if(pagebuf[i] != 0xFFFFFFFF){
            printf("%c", pagebuf[i]);
            tmp++;
        }
        else break;
    }

    //sectordata가 있는 경우에만 띄어쓰기 출력.
    if(tmp > 0) {
        printf(" ");
    }

    for(int i=SECTOR_SIZE; i<PAGE_SIZE; i++){
        if(pagebuf[i] == 0xFFFFFFFF) break;
        else printf("%c", pagebuf[i]);
    }


    fclose(flashfp);

}


void erase_block(char* filename, int pbn){
    flashfp = fopen(filename, "r+");
    dd_erase(pbn);
    fclose(flashfp);
}

int main(int argc, char *argv[])
{	
	char sectorbuf[SECTOR_SIZE];
	char pagebuf[PAGE_SIZE];
	char *blockbuf;

    if(strcmp(argv[1], "c") == 0){
        pbn = atoi(argv[3]);
        flash_memory_emulator(argv[2], pbn);
    }
    else if(strcmp(argv[1], "w") == 0){
        page_write(argv[2], atoi(argv[3]), argv[4], argv[5]);
    }
    else if(strcmp(argv[1], "r") == 0 ){
        page_read(argv[2], atoi(argv[3]));
    }
    else if(strcmp(argv[1], "e") == 0 ){
        erase_block(argv[2], atoi(argv[3]));
    }
	
	// flash memory 파일 생성: 위에서 선언한 flashfp를 사용하여 flash 파일을 생성한다. 그 이유는 fdevicedriver.c에서 
	//                 flashfp 파일포인터를 extern으로 선언하여 사용하기 때문이다.
	// 페이지 쓰기: pagebuf의 sector 영역과 spare 영역에 각각 데이터를 정확히 저장하고 난 후 해당 인터페이스를 호출한다.
	// 페이지 읽기: pagebuf를 인자로 사용하여 해당 인터페이스를 호출하여 페이지를 읽어 온 후 여기서 sector 데이터와
	//                  spare 데이터를 분리해 낸다
	// memset(), memcpy() 등의 함수를 이용하면 편리하다. 물론, 다른 방법으로 해결해도 무방하다.

	return 0;
}

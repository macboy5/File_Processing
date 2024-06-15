#include <stdio.h>		// 필요한 header file 추가 가능
#include "student.h"
#include <string.h>
#include <stdlib.h>

//
// readPage() 함수는 레코드 파일에서 주어진 페이지번호 pagenum(=0, 1, 2, ...)에 해당하는 page를
// 읽어서 pagebuf가 가리키는 곳에 저장하는 역할을 수행한다. 정상적인 수행을 한 경우
// '1'을, 그렇지 않은 경우는 '0'을 리턴한다.
// getRecFromPagebuf() 함수는 readPage()를 통해 읽어온 page에서 주어진 레코드번호 recordnum(=0, 1, 2, ...)에
// 해당하는 레코드를 recordbuf에 전달하는 일을 수행한다.
// 만약 page에서 전달할 record가 존재하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
// unpack() 함수는 recordbuf에 저장되어 있는 record에서 각 field를 추출하여 학생 객체에 저장하는 일을 한다.
//
int readPage(FILE *fp, char *pagebuf, int pagenum);
int getRecFromPagebuf(const char *pagebuf, char *recordbuf, int recordnum);
void unpack(const char *recordbuf, STUDENT *s);

//
// 주어진 학생 객체를 레코드 파일에 저장할 때 사용되는 함수들이며, 그 시나리오는 다음과 같다.
// 1. 학생 객체를 실제 레코드 형태의 recordbuf에 저장한다(pack() 함수 이용).
// 2. 레코드 파일의 맨마지막 page를 pagebuf에 읽어온다(readPage() 이용). 만약 새로운 레코드를 저장할 공간이
//    부족하면 pagebuf에 empty page를 하나 생성한다.
// 3. recordbuf에 저장되어 있는 레코드를 pagebuf의 page에 저장한다(writeRecToPagebuf() 함수 이용).
// 4. 수정된 page를 레코드 파일에 저장한다(writePage() 함수 이용)
// 
// writePage()는 성공적으로 수행하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int writePage(FILE *fp, const char *pagebuf, int pagenum);
void writeRecToPagebuf(char *pagebuf, const char *recordbuf);
void pack(char *recordbuf, const STUDENT *s);

//
// 레코드 파일에서 field의 키값(keyval)을 갖는 레코드를 검색하고 그 결과를 출력한다.
// 검색된 레코드를 출력할 때 반드시 printSearchResult() 함수를 사용한다.
//
void search(FILE *fp, FIELD f, char *keyval);
void printSearchResult(const STUDENT *s, int n);

//
// 레코드 파일에 새로운 학생 레코드를 추가할 때 사용한다. 표준 입력으로 받은 필드값들을 이용하여
// 학생 객체로 바꾼 후 insert() 함수를 호출한다.
// 당연히, 삭제 레코드 리스트에서 'right size'를 만족하는 삭제 레코드가 존재하면 여기에 새로운 레코드를 저장해야 한다.
// 이후 삭제 레코드 리스트의 수정이 필요하다.
//
void insert(FILE *fp, const STUDENT *s);

//
// 레코드의 필드명을 enum FIELD 타입의 값으로 변환시켜 준다.
// 예를 들면, 사용자가 수행한 명령어의 인자로 "NAME"이라는 필드명을 사용하였다면 
// 프로그램 내부에서 이를 NAME(=1)으로 변환할 필요성이 있으며, 이때 이 함수를 이용한다.
//
FIELD getFieldID(char *fieldname);

//
// 레코드 파일의 헤더를 읽어 오거나 수정할 때 사용한다.
// 예를 들어, 새로운 #pages를 수정하기 위해서는 먼저 readFileHeader()를 호출하여 헤더의 내용을
// headerbuf에 저장한 후 여기에 #pages를 수정하고 writeFileHeader()를 호출하여 변경된 헤더를 
// 저장한다. 두 함수 모두 성공하면 '1'을, 그렇지 않으면 '0'을 리턴한다.
//
int readFileHeader(FILE *fp, char *headerbuf);
int writeFileHeader(FILE *fp, const char *headerbuf);

//
// '학번' 키값을 갖는 학생 레코드를 삭제한다. 이때 삭제 레코드 리스트를 유지 관리하도록 구현되어야 한다.
//
void delete(FILE *fp, char *keyval);

// 
// '학번' 키값을 갖는 학생 레코드를 검색하여 해당 레코드를 recordbuf에 저장한다.
// 그리고 해당 레코드의 페이지 번호와 레코드 번호를 리턴한다. 이것 두 개가 필요한 이유는,
// 새로운 삭제 레코드가 생기면 삭제 레코드 리스트를 업데이트해야 하고 최종적으로 파일 헤더의
// pagenum와 recnum의 값을 각각 새로운 삭제 레코드의 pagenum와 recnum로 수정해야 하기 때문이다.
// 이 함수는 위의 delete() 함수에서 호출해서 사용한다.
// 검색 결과 레코드가 존재하면 '1', 그렇지 않으면 '0'을 리턴한다.
//
int searchByID(FILE *fp, char *keyval, char *recordbuf, int *pagenum, int *recordnum);

void make_record_file(FILE *fp, const char *file_name);

int ID_duplication_test(FILE *fp, const char *id);


int main(int argc, char *argv[])
{
    FILE *fp;						// 파일 처리 작업을 위한 C 라이브러리 파일 포인터

    fp = fopen(argv[2], "r+b");
    if(fp == NULL){
        make_record_file(fp, argv[2]);
        fp = fopen(argv[2], "r+b");
        if(fp == NULL){
            perror("Error opening file");
            exit(1);
        }
    }

    //레코드 저장 부분 구현
    if(strcmp(argv[1], "-i") == 0 ){

        if(argc != 10){
            fprintf(stderr, "need 7 fields of a record.\n");
            exit(1);
        }


        STUDENT s; //객체 생성
        for(int i=3; i<10 ; i++) {

            char field[strlen(argv[i]) + 1];
            char value[strlen(argv[i]) + 1];

            // field와 value를 초기화
            memset(field, 0, sizeof(field));
            memset(value, 0, sizeof(value));

            // argv[i] 값을 복사하여 field와 value로 분리
            int scanned = sscanf(argv[i], "%[^=]=%[^\n]", field, value);
//            printf("Field: %s, Value: %s\n", field, value);

            // value가 빈 문자열인지 확인하여 처리
            if (scanned != 2 || strlen(value) == 0) {
                fprintf(stderr, "Error: Invalid format or missing value for argument : %s\n", argv[i]);
                exit(1);
            }

            //value의 길이가 범위를 벗어났을 때 처리
            switch (*field) {
                case 3 :
                    if(strlen(value) > 8 ) {
                        fprintf(stderr, "ID value is out of range.\n");
                        exit(1);
                    }
                    break;
                case 4 :
                    if( strlen(value)>13) {
                        fprintf(stderr, "NAME value is out of range.\n");
                        exit(1);
                    }
                    break;
                case 5 :
                    if(strlen(value)>16) {
                        fprintf(stderr, "DEPT value is out of range.\n");
                        exit(1);
                    }
                    break;
                case 6 :
                    if(strlen(value) != 1) {
                        fprintf(stderr, "YEAR value is out of range.\n");
                        exit(1);
                    }
                    break;
                case 7 :
                    if(strlen(value)>20) {
                        fprintf(stderr, "ADDR value is out of range.\n");
                        exit(1);
                    }
                    break;
                case 8 :
                    if(strlen(value)>15) {
                        fprintf(stderr, "PHONE value is out of range.\n");
                        exit(1);
                    }
                    break;
                case 9 :
                    if( strlen(value)>20) {
                        fprintf(stderr, "EMAIL value is out of range.\n");
                        exit(1);
                    }
                    break;
                default:
                    break;
            }

            FIELD field_id = getFieldID(field);
            if (field_id == -1) {
                fprintf(stderr, "-i : Invalid field name: %s\n", field);
                exit(1);
            }
            switch (field_id) {
                case ID:
                    strncpy(s.id, value, sizeof(s.id) - 1);
                    s.id[sizeof(s.id) - 1] = '\0';
                    break;
                case NAME:
                    strncpy(s.name, value, sizeof(s.name) - 1);
                    s.name[sizeof(s.name) - 1] = '\0';
                    break;
                case DEPT:
                    strncpy(s.dept, value, sizeof(s.dept) - 1);
                    s.dept[sizeof(s.dept) - 1] = '\0';
                    break;
                case YEAR:
                    strncpy(s.year, value, sizeof(s.year) - 1);
                    s.year[sizeof(s.year) - 1] = '\0';
                    break;
                case ADDR:
                    strncpy(s.addr, value, sizeof(s.addr) - 1);
                    s.addr[sizeof(s.addr) - 1] = '\0';
                    break;
                case PHONE:
                    strncpy(s.phone, value, sizeof(s.phone) - 1);
                    s.phone[sizeof(s.phone) - 1] = '\0';
                    break;
                case EMAIL:
                    strncpy(s.email, value, sizeof(s.email) - 1);
                    s.email[sizeof(s.email) - 1] = '\0';
                    break;
                default:
                    fprintf(stderr, "Invalid field.\n");
                    exit(1);

            }
        }

        //insert() 실행 전,,, 레코드 파일에 해당 학번이 이미 존재 하는지 아닌지 검사.
        //존재한다면...에러 처리

        if (ID_duplication_test(fp, s.id)) {
            fprintf(stderr, "Error: A record with ID %s already exists.\n", s.id);
            exit(1);
        }

        insert(fp, &s);

    }


        //레코드 내 검색 구현
    else if(strcmp(argv[1], "-s") ==0 ) {

        if(argc!=4) {
            fprintf(stderr, "need only 1 field as a key.\n");
            exit(1);
        }



        char field[strlen(argv[3]) + 1];
        char value[strlen(argv[3]) + 1];

        // field와 value를 초기화
        memset(field, 0, sizeof(field));
        memset(value, 0, sizeof(value));


//      // argv[i] 값을 복사하여 field와 value로 분리
        int scanned = sscanf(argv[3], "%[^=]=%[^\n]", field, value);
//        printf("Field: %s, Value: %s\n", field, value);

        if (scanned != 2 || strlen(value) == 0) {
            fprintf(stderr, "Error: Invalid format or missing value for argument : %s\n", argv[3]);
            exit(1);
        }


//            char *equal_sign = strchr(argv[3], '=');
//            if (equal_sign == NULL) {
//                fprintf(stderr, "Error: Invalid format. Missing '='\n");
//                return 1;
//            }

//            strncpy(field, argv[3], equal_sign - argv[3]);
//            strcpy(value, equal_sign + 1);

        // Check if value is empty
//            if (strlen(value) == 0) {
//                fprintf(stderr, "Error: Value for field %s is missing.\n", field);
//                return 1;
//            }


        FIELD field_id = getFieldID(field);
        if (field_id == -1) {
            fprintf(stderr, "-s : Invalid field name: %s\n", field);
            exit(1);
        }

        search(fp, field_id, value);

    }

    else if(strcmp(argv[1], "-d") ==0 ){

        if(argc!=4) {
            fprintf(stderr, "need only 1 ID as a key.\n");
            exit(1);
        }

        char ID[strlen(argv[3]) + 1];
        char value[strlen(argv[3]) + 1];

        // field와 value를 초기화
        memset(ID, 0, sizeof(ID));
        memset(value, 0, sizeof(value));


//      // argv[i] 값을 복사하여 field와 value로 분리
        int scanned = sscanf(argv[3], "%[^=]=%[^\n]", ID, value);
//        printf("Field: %s, Value: %s\n", ID, value);

        if (scanned != 2 || strlen(value) == 0) {
            fprintf(stderr, "Error: Invalid format or missing value for argument : %s\n", argv[3]);
            exit(1);
        }


        delete(fp, value);

    }

    else{
        fprintf(stderr, "only -i or -s is available for first the argument\n");
        exit(1);
    }

    fclose(fp);

    return 1;
}





void make_record_file(FILE *fp, const char *file_name){
    //처음 레코드 파일 생성 시. 헤더만 생성.
    fp = fopen(file_name, "w+b");
    if (fp == NULL) {
        perror("make_record_file() : Error creating file");
        exit(1);
    }

    char headerbuf[FILE_HEADER_SIZE];
    memset(headerbuf, 0xFF, FILE_HEADER_SIZE);

    //headerbuf에 total_pages 적어줌.
    unsigned short total_pages = 0; // including header page
    memcpy(headerbuf, &total_pages, sizeof(total_pages));

    //레코드 파일의 헤더영역의 reserved space의 2B, 2B에 page_num = -1, record_num = -1로 초기화
    unsigned short page_num = -1;
    memcpy(headerbuf+2, &page_num, sizeof(page_num));
    unsigned short record_num = -1;
    memcpy(headerbuf+4, &record_num, sizeof(record_num));


    if (!writeFileHeader(fp, headerbuf)) {
        fprintf(stderr, "make_record_file() : Failed to write file header\n");
        exit(1);
    }

    fclose(fp);
}

int readPage(FILE *fp, char *pagebuf, int pagenum){
    if (fseek(fp, FILE_HEADER_SIZE + pagenum * PAGE_SIZE, SEEK_SET) != 0) {
        perror("readPage() : Error seeking to page location");
        return 0;
    }

    size_t bytesRead = fread(pagebuf, 1, PAGE_SIZE, fp);
    if (bytesRead != PAGE_SIZE) {
        fprintf(stderr, "readPage() : Error reading page: expected %d bytes, read %zu bytes\n", PAGE_SIZE, bytesRead);
        return 0;
    }

//    printf("readPage() test\n");
//    for (int i = 0; i < PAGE_SIZE; i++) {
//        printf("%02x ", pagebuf[i]);
//    }
//    printf("\n");

    return 1;

}

int getRecFromPagebuf(const char *pagebuf, char *recordbuf, int recordnum){

    //Page의 header area의 첫 2 byte에 저장되어있는 #records 정보.
    unsigned short total_records;
    memcpy(&total_records, pagebuf, sizeof(total_records));

    //전체레코드 수보다 큰 recordnum이 들어오면 error
    if(recordnum >= total_records){
        fprintf(stderr, "getRecFromPagebuf() : recordnum is bigger than total record number\n");
        return 0;
    }
    if(recordnum > 28 || recordnum < 0) {
        fprintf(stderr, "getRecFromPagebuf() : recordnum is out of valid range.\n");
        return 0;
    }


    if(recordnum == 0){ //0번째 레코드를 읽어라.
        unsigned short offset;
        memcpy(&offset, pagebuf+8, sizeof(offset));
        memcpy(recordbuf, pagebuf+PAGE_HEADER_SIZE, offset+1);

        return 1;
    }
    else{  // recordnum번째 레코드를 읽어라.
        unsigned short offset1;
        memcpy(&offset1, pagebuf + 8 + (recordnum-1)*2, sizeof(offset1));
        unsigned short offset2;
        memcpy(&offset2, pagebuf + 8 + recordnum * 2, sizeof(offset2));

//        if(offset2+1 > PAGE_SIZE-PAGE_HEADER_SIZE) {
//            fprintf(stderr, "data area size is 448 byte, so offset cannot bigger than 447\n");
//            return 0;
//        }

        memcpy(recordbuf, pagebuf + PAGE_HEADER_SIZE+offset1+1, offset2-offset1);

        return 1;
    }


}

void unpack(const char *recordbuf, STUDENT *s){
    sscanf(recordbuf, "%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]#%[^#]",
           s->id, s->name, s->dept, s->year, s->addr, s->phone, s->email);
}




int writePage(FILE *fp, const char *pagebuf, int pagenum){
    if (fseek(fp, FILE_HEADER_SIZE + pagenum * PAGE_SIZE, SEEK_SET) != 0) {
        perror("writePage() : Error seeking to page location");
        return 0;
    }
    if (fwrite(pagebuf, 1, PAGE_SIZE, fp) != PAGE_SIZE) {
        fprintf(stderr, "writePage() : Error writing page\n");
        return 0;
    }

//    printf("writePage() test \n");
//    for(int i=0; i<PAGE_SIZE; i++){
//        printf("%02x ", pagebuf[i]);
//    }
//    printf("\n");

    return 1;

}

void writeRecToPagebuf(char *pagebuf, const char *recordbuf) {
    //pagebuf를 불러와서...다양한 검사를 진행..
    //recordbuf를 pagebuf에 적어야됨.

    unsigned short total_records=0;
    memcpy(&total_records, pagebuf, sizeof(total_records));

    unsigned short free_space;
    memcpy(&free_space, pagebuf+2, sizeof(free_space));

    int no_fit = 1;

    //페이지 내에 적혀있던 마지막 레코드 다음 offset에 적는다.
    unsigned short offset;
    if(total_records == 0){//처음 적는 경우
        offset = 0;
        memcpy(pagebuf+PAGE_HEADER_SIZE, recordbuf, strlen(recordbuf));
    }
    else {
        //first-fit 전략을 사용. 만약에 넣을 수 있는 곳에 없으면 맨뒤에 append.
        for (int i = 0; i < total_records; i++) {
            //'*' 마킹되어있는지 검사.
            int del_marker;
            unsigned short offset_curr;
            if (i == 0) {
                memcpy(&del_marker, pagebuf + 8, 1);
            }else {
                memcpy(&offset, pagebuf + 8 + 2 * (i - 1), 2);
                memcpy(&del_marker, pagebuf + PAGE_HEADER_SIZE + offset + 1, 1);
            }

            //길이에 대한 비교..
            if (del_marker == '*' && i <total_records-1) {
                if(i == 0 ){
                    memcpy(&offset_curr, pagebuf + 8 , 2);
                    //적을 수 있는 경우
                    if( strlen(recordbuf) <= offset_curr+1){
                        no_fit=0;
                        memcpy(pagebuf + PAGE_HEADER_SIZE, recordbuf, strlen(recordbuf));
                    }
                }
                else {
                    memcpy(&offset, pagebuf + 8 + 2 * (i - 1), 2);
                    memcpy(&offset_curr, pagebuf + 8 + 2 * i, 2);
                    //적을 수 있는 경우..
                    if (strlen(recordbuf) <= offset_curr - offset-1) {
                        no_fit = 0;
                        memcpy(pagebuf + PAGE_HEADER_SIZE + offset_curr + 1, recordbuf, strlen(recordbuf));
                    }

                }
            }

            //맨 마지막 record가 지워진 경우..
            if(del_marker == '*' && i == total_records-1) {
                //적을 수 있는 경우.
                if (offset + strlen(recordbuf) <= PAGE_SIZE - PAGE_HEADER_SIZE) {
                    no_fit = 0;
                    memcpy(pagebuf + PAGE_HEADER_SIZE + offset_curr + 1, recordbuf, strlen(recordbuf));
                }
                else{
                    //다음 빈 페이지에 적어야하는 경우.
                    //empty pagebuf를 만들고 거기에 써서 넘겨줘야함.
                    no_fit = 1;

                    memset(pagebuf, 0xFF, PAGE_SIZE); //empty pagebuf로 만들어줌.

                    //recordbuf를 pagebuf에 쓴다.
                    memcpy(pagebuf + PAGE_HEADER_SIZE, recordbuf, strlen(recordbuf));

                    //헤더 업데이트
                    total_records = 1;
                    memcpy(pagebuf, &total_records, sizeof(total_records));

                    free_space=PAGE_SIZE-PAGE_HEADER_SIZE- strlen(recordbuf);
                    memcpy(pagebuf+2, &free_space, sizeof(free_space));

                    unsigned short offset_by_len = strlen(recordbuf)-1;
                    memcpy(pagebuf+8, &offset_by_len, sizeof(offset_by_len));


                }
            }
        }

    }

    //del된 곳이 없을 경우 --> 맨 뒤에 append.. 맨 뒤에 쓸 수 없다면 다음 페이지로 넘겨줘야함.
    if (no_fit) {
        if(strlen(recordbuf) > free_space){
            //다음 빈 페이지에 적어야하는 경우.
            //empty pagebuf를 만들고 거기에 써서 넘겨줘야함.

            memset(pagebuf, 0xFF, PAGE_SIZE); //empty pagebuf로 만들어줌.

            //recordbuf를 pagebuf에 쓴다.
            memcpy(pagebuf + PAGE_HEADER_SIZE, recordbuf, strlen(recordbuf));

            //헤더 업데이트
            total_records = 1;
            memcpy(pagebuf, &total_records, sizeof(total_records));

            free_space=PAGE_SIZE-PAGE_HEADER_SIZE- strlen(recordbuf);
            memcpy(pagebuf+2, &free_space, sizeof(free_space));

            unsigned short offset_by_len = strlen(recordbuf)-1;
            memcpy(pagebuf+8, &offset_by_len, sizeof(offset_by_len));
        }
        else{
            //맨 뒤에 append 가능한 경우
            // total_record + 1 해줘야됨.
            total_records++;
            memcpy(pagebuf, &total_records, sizeof(total_records));


            //새롭게 적은 레코드의 마지막 offset을 기록해줘야됨.
            unsigned short new_offset;
            if (offset == 0) {
                new_offset = offset + strlen(recordbuf) - 1;
            } else {
                new_offset = offset + strlen(recordbuf);
            }

            memcpy(pagebuf + 8 + (total_records - 1) * 2, &new_offset, sizeof(new_offset));

            //free_space 값 update
            free_space = PAGE_SIZE - PAGE_HEADER_SIZE - new_offset - 1;
            memcpy(pagebuf + 2, &free_space, sizeof(free_space));


            memcpy(&offset, pagebuf + 8 + (total_records - 1) * 2, sizeof(offset));
            memcpy(pagebuf + PAGE_HEADER_SIZE + offset + 1, recordbuf, strlen(recordbuf));

        }
    }



}

void pack(char *recordbuf, const STUDENT *s){

    int n = snprintf(recordbuf,  MAX_RECORD_SIZE, "%s#%s#%s#%s#%s#%s#%s#",
                     s->id, s->name, s->dept, s->year, s->addr, s->phone, s->email);
    if(n >= MAX_RECORD_SIZE){
        fprintf(stderr, "pack() : Record buffer size exceeded. Actual size : %d\n", n);
    }
//    printf("pack() this is recorbuf: %s\n", recordbuf);
}



void search(FILE *fp, FIELD f, char *keyval){
    //레코드 파일에서 field의 키값(keyval)을 갖는 레코드를 검색
    char headerbuf[FILE_HEADER_SIZE];
    if (!readFileHeader(fp, headerbuf)) {
        fprintf(stderr, "search() : Failed to read file header\n");
        return;
    }

    unsigned short total_pages;
    memcpy(&total_pages, headerbuf, sizeof(total_pages));
//    printf("search() : Total pages: %d\n", total_pages);

    STUDENT s;
    int found_count = 0;
    STUDENT found_records[1000];

    for(int i=0; i<total_pages; i++){
        char pagebuf[PAGE_SIZE];
        if (!readPage(fp, pagebuf, i)) {
            fprintf(stderr, "search() : Failed to read page %d\n", i);
            continue;
        }


        unsigned short total_records;
        memcpy(&total_records, pagebuf, sizeof(total_records));
//        printf("Page %d: Total records: %d\n", i, total_records);


        for(int j=0; j<total_records; j++){
            char recordbuf[MAX_RECORD_SIZE];
            if (!getRecFromPagebuf(pagebuf, recordbuf, j)) {
                fprintf(stderr, "search() : Failed to get record %d from page %d\n", j, i);
                continue;
            }

            unpack(recordbuf, &s);


            char *field_value = NULL;
            switch (f) {
                case ID: field_value = s.id; break;
                case NAME: field_value = s.name; break;
                case DEPT: field_value = s.dept; break;
                case YEAR: field_value = s.year; break;
                case ADDR: field_value = s.addr; break;
                case PHONE: field_value = s.phone; break;
                case EMAIL: field_value = s.email; break;
                default:
                    fprintf(stderr, "search() : Invalid field.\n");
                    return;
            }

//                printf("Comparing field value '%s' with key value '%s'\n", field_value, keyval);


            if (field_value && strcmp(field_value, keyval) == 0) {
                if (found_count < 1000) { // found_records 배열의 크기 제한 체크
                    found_records[found_count++] = s;
                } else {
                    fprintf(stderr, "search() : Found record limit reached at 1000.\n");
                    break;
                }
            }

        }

    }

    printSearchResult(found_records, found_count);
}

void printSearchResult(const STUDENT *s, int n)
{
    int i;

    printf("#Records = %d\n", n);

    for(i=0; i<n; i++)
    {
        printf("%s#%s#%s#%s#%s#%s#%s\n", s[i].id, s[i].name, s[i].dept, s[i].year, s[i].addr, s[i].phone, s[i].email);
    }
}



int ID_duplication_test(FILE *fp, const char *id) {
    char headerbuf[FILE_HEADER_SIZE];
    if (!readFileHeader(fp, headerbuf)) {
        fprintf(stderr, "Failed to read file header\n");
        return 0;
    }

    unsigned short total_pages;
    memcpy(&total_pages, headerbuf, sizeof(total_pages));

    STUDENT s;

    for (int i = 0; i < total_pages; i++) {
        char pagebuf[PAGE_SIZE];
        if (!readPage(fp, pagebuf, i)) {
            fprintf(stderr, "checkDUPID() : Failed to read page %d\n", i);
            continue;
        }

        unsigned short total_records;
        memcpy(&total_records, pagebuf, sizeof(total_records));

        for (int j = 0; j < total_records; j++) {
            char recordbuf[MAX_RECORD_SIZE];
            if (!getRecFromPagebuf(pagebuf, recordbuf, j)) {
                fprintf(stderr, "Failed to get record %d from page %d\n", j, i);
                continue;
            }

            unpack(recordbuf, &s);

            if (strcmp(s.id, id) == 0) {
                return 1; // 중복 ID, 이미 ID가 존재할 ?? 1 return.
            }
        }
    }

    return 0; // No duplicate ID found
}



void insert(FILE *fp, const STUDENT *s){

    char headerbuf[FILE_HEADER_SIZE];
    if (!readFileHeader(fp, headerbuf)) {
        fprintf(stderr, "Failed to read file header\n");
        return;
    }

    unsigned short total_pages;
    memcpy(&total_pages, headerbuf, sizeof(total_pages));

    char pagebuf[PAGE_SIZE];
    char recordbuf[MAX_RECORD_SIZE];
    pack(recordbuf, s);

    int last_page_num = 0;
    unsigned short free_space=PAGE_SIZE-PAGE_HEADER_SIZE;
    unsigned short total_records = 0;

    //첫 번째 레코드 저장의 경우 읽어올 페이지가 없으니,,,
    //처음 insert할때, 페이지가 아직 없으므로 readPage() 생략..
    if(total_pages > 0) {
        last_page_num = total_pages - 1;
        if (!readPage(fp, pagebuf, last_page_num)) {  //마지막 페이지의 pagebuf를 가져온다.
            fprintf(stderr, "insert() : Failed to read last page\n");
            return;
        }
//        memcpy(&free_space, pagebuf+2, sizeof(free_space));
//        memcpy(&total_records, pagebuf, sizeof(total_records));

        writeRecToPagebuf(pagebuf, recordbuf);
        memcpy(&total_records, pagebuf, sizeof(total_records));

        if(total_records ==  1){
            total_pages++;
            last_page_num = total_pages-1;
            memcpy(headerbuf, &total_pages, sizeof(total_pages));
            writeFileHeader(fp, headerbuf);
        }
        writePage(fp, pagebuf, last_page_num);

    }
    else if(total_pages == 0){
        // 새로운 페이지를 생성하는 경우 페이지를 초기화
        memset(pagebuf, 0xFF, PAGE_SIZE); // empty pagebuf로 만들어줌

        memcpy(pagebuf, &total_records, sizeof(total_records));

        memcpy(pagebuf+2, &free_space, sizeof(free_space));
        total_pages = 1;
        last_page_num = 0;

        writeRecToPagebuf(pagebuf, recordbuf);
        writePage(fp, pagebuf, last_page_num);
    }



//    //해당 페이지에 쓸 수 없는 경우. 다음 페이지에 써야하는 경우..다음 페이지에 쓴다.
//    if(free_space <  strlen(recordbuf) ){
//        total_pages++;
//        last_page_num = total_pages - 1;
//        memset(pagebuf, 0xFF, PAGE_SIZE); //empty pagebuf로 만들어줌.
//        total_records = 0;
//        memcpy(pagebuf, &total_records, sizeof(total_records));
//        free_space=PAGE_SIZE-PAGE_HEADER_SIZE;
//        memcpy(pagebuf+2, &free_space, sizeof(free_space));
//
//    }

//    writeRecToPagebuf(pagebuf, recordbuf);
//    writePage(fp, pagebuf, last_page_num);

//    // 헤더 업데이트 : total_record == 1 인경우.. 즉 완전히 처음에 쓰는 경우 혹은 다음 페이지에 쓴 경우..
//    if (total_records == 1) {
//
//        memcpy(headerbuf, &total_pages, sizeof(total_pages));
//        writeFileHeader(fp, headerbuf);
//    }



}


FIELD getFieldID(char *fieldname){
    if (strcmp(fieldname, "ID") == 0) return ID;
    if (strcmp(fieldname, "NAME") == 0) return NAME;
    if (strcmp(fieldname, "DEPT") == 0) return DEPT;
    if (strcmp(fieldname, "YEAR") == 0) return YEAR;
    if (strcmp(fieldname, "ADDR") == 0) return ADDR;
    if (strcmp(fieldname, "PHONE") == 0) return PHONE;
    if (strcmp(fieldname, "EMAIL") == 0) return EMAIL;
    return -1;

//    if (strcasecmp(fieldname, "id") == 0) return ID;
//    if (strcasecmp(fieldname, "name") == 0) return NAME;
//    if (strcasecmp(fieldname, "dept") == 0) return DEPT;
//    if (strcasecmp(fieldname, "year") == 0) return YEAR;
//    if (strcasecmp(fieldname, "addr") == 0) return ADDR;
//    if (strcasecmp(fieldname, "phone") == 0) return PHONE;
//    if (strcasecmp(fieldname, "email") == 0) return EMAIL;
//    return -1;

}



int readFileHeader(FILE *fp, char *headerbuf){
    if(fseek(fp, 0, SEEK_SET) != 0) return 0;
    //header의 맨 앞 2B에 전체 페이지수가 저장되어 있음.

    //헤더 읽어오기
    if (fread(headerbuf, 1, FILE_HEADER_SIZE, fp) != FILE_HEADER_SIZE) {
        return 0;
    }

//    printf("readFileHearder() test\n");
//    for(int i=0; i<FILE_HEADER_SIZE; i++){
//        printf("%02x ", headerbuf[i]);
//    }
//    printf("\n");

    return 1;
}

int writeFileHeader(FILE *fp, const char *headerbuf) {
    if(fseek(fp, 0, SEEK_SET) != 0) return 0;
    if (fwrite(headerbuf, 1, FILE_HEADER_SIZE, fp) != FILE_HEADER_SIZE) {
        return 0;
    }

//    printf("writeFileHearder() test\n");
//    for (int i = 0; i < FILE_HEADER_SIZE; i++) {
//        printf("%02x ", headerbuf[i]);
//    }
//    printf("\n");

    return 1;
}

void delete(FILE *fp, char *keyval){
    char recordbuf[MAX_RECORD_SIZE];

    //레코드 파일에서 field의 키값(keyval)을 갖는 레코드를 검색 ... (field, keyval)
    char headerbuf[FILE_HEADER_SIZE];
    if (!readFileHeader(fp, headerbuf)) {
        fprintf(stderr, "search() : Failed to read file header\n");
        return;
    }

    //레코드 파일의 reserved area에 적혀있는 직전의 삭제 위치 가져오기.
    unsigned short pagenum, recordnum;
    memcpy(&pagenum, headerbuf+2, sizeof(pagenum));
    memcpy(&recordnum, headerbuf+4, sizeof(recordnum));

    unsigned short total_pages;
    memcpy(&total_pages, headerbuf, sizeof(total_pages));
//    printf("search() : Total pages: %d\n", total_pages);

    STUDENT s;

    for(int i=0; i<total_pages; i++){
        char pagebuf[PAGE_SIZE];
        if (!readPage(fp, pagebuf, i)) {
            fprintf(stderr, "search() : Failed to read page %d\n", i);
            continue;
        }


        unsigned short total_records;
        memcpy(&total_records, pagebuf, sizeof(total_records));
//        printf("Page %d: Total records: %d\n", i, total_records);


        for(int j=0; j<total_records; j++){

            //일치하는 ID를 레코드 내에서 찾은 경우.. 삭제 작업 진행
            //일치하는 ID가 있는 recordbuf를 가져왔다..
            if(searchByID(fp, keyval, recordbuf, &i, &j)){

                //recordbuf 내용을 *PagenumRecordnum으로 덮어쓰기
                //pagenum은 직전에 삭제되었던 페이지 번호, recordnum은 직전에 삭제되었던 레코드 번호..
                //레코드 파일 헤더에 저장되어 있던 값을 가져와서 업데이트해주고
                //레코드 파일의 헤더에 지금 삭제되는 페이지번호, 레코드 번호로 업데이트.
                memset(recordbuf, 0xFF, strlen(recordbuf));

                memset(recordbuf, '*', 1);
                memset(recordbuf+1, pagenum, 2);
                memset(recordbuf+3, recordnum, 2);

                memset(headerbuf+2, i, 2);
                memset(headerbuf+4, j ,2);
                writeFileHeader(fp, headerbuf);

                //덮어쓴 recordbuf를 기존 위치에 찾아가서 업데이트
                //pagebuf에서 offset을 구해서 그곳에 update..

                if(j == 0){ //첫번째 레코드가 삭제되는 경우
                    memcpy(pagebuf+PAGE_HEADER_SIZE, recordbuf, strlen(recordbuf));
                }
                else {
                    unsigned short offset;
                    memcpy(&offset, pagebuf + 8 + 2 * (j - 1), sizeof(offset));
                    memcpy(pagebuf+PAGE_HEADER_SIZE+offset+1, recordbuf, strlen(recordbuf));
                }
                writePage(fp, pagebuf, i);

                return;

            }
        }
    }



}

int searchByID(FILE *fp, char *keyval, char *recordbuf, int *pagenum, int *recordnum){

    //pagenum의 recordnum으로 가서 검사를 진행.
    char pagebuf[PAGE_SIZE];
    readPage(fp, pagebuf, *pagenum);

    char record_buf[MAX_RECORD_SIZE];
    getRecFromPagebuf(pagebuf, recordbuf, *recordnum);

    STUDENT s;

    unpack(record_buf, &s);

    if(s.id == keyval){
        //찾았으면 해당 레코드를 recordbuf에 저장.
        memcpy(recordbuf, record_buf, sizeof(record_buf));

        printSearchResult(&s, 1);

        return 1;
    }


    //못찾은 경우에는 0을 return.

    return 0;
}



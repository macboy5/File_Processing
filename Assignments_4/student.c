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

void make_record(FILE *fp, const char *file_name);

int checkDuplicateID(FILE *fp, const char *id);


int main(int argc, char *argv[])
{
	FILE *fp;						// 모든 file processing operation은 C library를 사용할 것

    fp = fopen(argv[2], "r+b");
    if(fp == NULL){
       // fclose(fp);
        make_record(fp, argv[2]);
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

            // argv[i] 값을 복사하여 field와 value로 분리
            sscanf(argv[i], "%[^=]=%[^\0]", field, value);
//            printf("Field: %s, Value: %s\n", field, value);

            FIELD field_id = getFieldID(field);
            if (field_id == -1) {
                fprintf(stderr, "-i : Invalid field name: %s\n", field);
                exit(1);
            }
            switch (field_id) {
                case ID:
                    strncpy(s.id, value, sizeof(s.id) - 1);
                    s.id[sizeof(s.id) - 1] = '\0';  // Ensure null-terminated
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

        if (checkDuplicateID(fp, s.id)) {
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

        // argv[i] 값을 복사하여 field와 value로 분리
        sscanf(argv[3], "%[^=]=%[^\0]", field, value);
//        printf("Field: %s, Value: %s\n", field, value);

        FIELD field_id = getFieldID(field);
        if (field_id == -1) {
            fprintf(stderr, "-s : Invalid field name: %s\n", field);
            exit(1);
        }

        search(fp, field_id, value);

    }

    else{
        fprintf(stderr, "only -i or -s is available for first the argument\n");
        exit(1);
    }

    fclose(fp);

	return 1;
}





void make_record(FILE *fp, const char *file_name){
    //처음 레코드 파일 생성 시. 헤더만 생성.
    fp = fopen(file_name, "w+b");
    if (fp == NULL) {
        perror("make_record() : Error creating file");
        exit(1);
    }

    //headerbuf에 total_pages 적어줌.
    char headerbuf[FILE_HEADER_SIZE];
    memset(headerbuf, 0xFF, FILE_HEADER_SIZE);

    unsigned short total_pages = 0; // including header page
    memcpy(headerbuf, &total_pages, sizeof(total_pages));

    if (!writeFileHeader(fp, headerbuf)) {
        fprintf(stderr, "make_record() : Failed to write file header\n");
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
    else{ // recordnum번째 레코드를 읽어라..
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
    //recordbuf를 pagebuf에 적어야됨.

    unsigned short total_records=0;
    memcpy(&total_records, pagebuf, sizeof(total_records));

    unsigned short free_space;
    memcpy(&free_space, pagebuf+2, sizeof(free_space));

    //페이지 내에 적혀있던 마지막 레코드 다음 offset에 적는다.
    unsigned short offset;
    if(total_records == 0){//처음 적는 경우
        offset = 0;
        memcpy(pagebuf+PAGE_HEADER_SIZE, recordbuf, strlen(recordbuf));
    }
    else {
        memcpy(&offset, pagebuf + 8 + (total_records - 1) * 2, sizeof(offset));
        memcpy(pagebuf+PAGE_HEADER_SIZE+offset+1, recordbuf, strlen(recordbuf));
    }

    //적었으므로 total_record + 1 해줘야됨.
    total_records++;
    memcpy(pagebuf, &total_records, sizeof(total_records));


    //새롭게 적은 레코드의 마지막 offset을 기록해줘야됨.
    unsigned short new_offset;
    if(offset ==0) {
        new_offset = offset + strlen(recordbuf) - 1;
    }
    else{
        new_offset = offset+strlen(recordbuf);
    }

    memcpy(pagebuf + 8 + (total_records-1) * 2, &new_offset, sizeof(new_offset));

    //free_space 값 update
    free_space = PAGE_SIZE-PAGE_HEADER_SIZE - new_offset-1;
    memcpy(pagebuf+2, &free_space, sizeof(free_space));

    //테스트 코드
//    printf("writeRecToPagebuf() test\n");
//    for(int i=0; i<PAGE_SIZE; i++){
//        printf("%02x ", pagebuf[i]);
//    }
//    printf("\n");

}

void pack(char *recordbuf, const STUDENT *s){

    snprintf(recordbuf,  MAX_RECORD_SIZE, "%s#%s#%s#%s#%s#%s#%s#",
             s->id, s->name, s->dept, s->year, s->addr, s->phone, s->email);
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

void printSearchResult(const STUDENT *s, int n){
    int i;
    printf("#Records = %d\n", n);
    for(i=0; i<n; i++){
        printf("%s#%s#%s#%s#%s#%s#%s\n", s[i].id, s[i].name, s[i].dept, s[i].year, s[i].addr, s[i].phone, s[i].email);
    }
}


int checkDuplicateID(FILE *fp, const char *id) {
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
                return 1; // Duplicate ID found
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
        if (!readPage(fp, pagebuf, last_page_num)) { //pagebuf를 가져온다.
            fprintf(stderr, "insert() : Failed to read last page\n");
            return;
        }
        memcpy(&free_space, pagebuf+2, sizeof(free_space));
        memcpy(&total_records, pagebuf, sizeof(total_records));
    }
    else if(total_pages == 0){
        // 새로운 페이지를 생성하는 경우 페이지를 초기화
        memset(pagebuf, 0xFF, PAGE_SIZE); // empty pagebuf로 만들어줌

        memcpy(pagebuf, &total_records, sizeof(total_records));

        memcpy(pagebuf+2, &free_space, sizeof(free_space));
        total_pages = 1;
        last_page_num = 0;
    }



    //해당 페이지에 쓸 수 없는 경우. 다음 페이지에 써야하는 경우
    if(free_space <  strlen(recordbuf) ){
        total_pages++;
        last_page_num = total_pages - 1;
        memset(pagebuf, 0xFF, PAGE_SIZE); //empty pagebuf로 만들어줌.
        total_records = 0;
        memcpy(pagebuf, &total_records, sizeof(total_records));
        free_space=PAGE_SIZE-PAGE_HEADER_SIZE;
        memcpy(pagebuf+2, &free_space, sizeof(free_space));

    }

    writeRecToPagebuf(pagebuf, recordbuf);
    writePage(fp, pagebuf, last_page_num);

    // 헤더 업데이트
    if (total_records == 0 || free_space < strlen(recordbuf)) {
        memcpy(headerbuf, &total_pages, sizeof(total_pages));
        writeFileHeader(fp, headerbuf);
    }



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

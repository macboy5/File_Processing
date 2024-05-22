#include <stdio.h>		// �ʿ��� header file �߰� ����
#include "student.h"
#include <string.h>
#include <stdlib.h>

//
// readPage() �Լ��� ���ڵ� ���Ͽ��� �־��� ��������ȣ pagenum(=0, 1, 2, ...)�� �ش��ϴ� page��
// �о pagebuf�� ����Ű�� ���� �����ϴ� ������ �����Ѵ�. �������� ������ �� ���
// '1'��, �׷��� ���� ���� '0'�� �����Ѵ�.
// getRecFromPagebuf() �Լ��� readPage()�� ���� �о�� page���� �־��� ���ڵ��ȣ recordnum(=0, 1, 2, ...)��
// �ش��ϴ� ���ڵ带 recordbuf�� �����ϴ� ���� �����Ѵ�.
// ���� page���� ������ record�� �����ϸ� '1'��, �׷��� ������ '0'�� �����Ѵ�.
// unpack() �Լ��� recordbuf�� ����Ǿ� �ִ� record���� �� field�� �����Ͽ� �л� ��ü�� �����ϴ� ���� �Ѵ�.
//
int readPage(FILE *fp, char *pagebuf, int pagenum);
int getRecFromPagebuf(const char *pagebuf, char *recordbuf, int recordnum);
void unpack(const char *recordbuf, STUDENT *s);

//
// �־��� �л� ��ü�� ���ڵ� ���Ͽ� ������ �� ���Ǵ� �Լ����̸�, �� �ó������� ������ ����.
// 1. �л� ��ü�� ���� ���ڵ� ������ recordbuf�� �����Ѵ�(pack() �Լ� �̿�).
// 2. ���ڵ� ������ �Ǹ����� page�� pagebuf�� �о�´�(readPage() �̿�). ���� ���ο� ���ڵ带 ������ ������
//    �����ϸ� pagebuf�� empty page�� �ϳ� �����Ѵ�.
// 3. recordbuf�� ����Ǿ� �ִ� ���ڵ带 pagebuf�� page�� �����Ѵ�(writeRecToPagebuf() �Լ� �̿�).
// 4. ������ page�� ���ڵ� ���Ͽ� �����Ѵ�(writePage() �Լ� �̿�)
// 
// writePage()�� ���������� �����ϸ� '1'��, �׷��� ������ '0'�� �����Ѵ�.
//
int writePage(FILE *fp, const char *pagebuf, int pagenum);
void writeRecToPagebuf(char *pagebuf, const char *recordbuf);
void pack(char *recordbuf, const STUDENT *s);

//
// ���ڵ� ���Ͽ��� field�� Ű��(keyval)�� ���� ���ڵ带 �˻��ϰ� �� ����� ����Ѵ�.
// �˻��� ���ڵ带 ����� �� �ݵ�� printSearchResult() �Լ��� ����Ѵ�.
//
void search(FILE *fp, FIELD f, char *keyval);
void printSearchResult(const STUDENT *s, int n);

//
// ���ڵ� ���Ͽ� ���ο� �л� ���ڵ带 �߰��� �� ����Ѵ�. ǥ�� �Է����� ���� �ʵ尪���� �̿��Ͽ�
// �л� ��ü�� �ٲ� �� insert() �Լ��� ȣ���Ѵ�.
//
void insert(FILE *fp, const STUDENT *s);

//
// ���ڵ��� �ʵ���� enum FIELD Ÿ���� ������ ��ȯ���� �ش�.
// ���� ���, ����ڰ� ������ ��ɾ��� ���ڷ� "NAME"�̶�� �ʵ���� ����Ͽ��ٸ� 
// ���α׷� ���ο��� �̸� NAME(=1)���� ��ȯ�� �ʿ伺�� ������, �̶� �� �Լ��� �̿��Ѵ�.
//
FIELD getFieldID(char *fieldname);

//
// ���ڵ� ������ ����� �о� ���ų� ������ �� ����Ѵ�.
// ���� ���, ���ο� #pages�� �����ϱ� ���ؼ��� ���� readFileHeader()�� ȣ���Ͽ� ����� ������
// headerbuf�� ������ �� ���⿡ #pages�� �����ϰ� writeFileHeader()�� ȣ���Ͽ� ����� ����� 
// �����Ѵ�. �� �Լ� ��� �����ϸ� '1'��, �׷��� ������ '0'�� �����Ѵ�.
//
int readFileHeader(FILE *fp, char *headerbuf);

int writeFileHeader(FILE *fp, const char *headerbuf);

void make_record(FILE *fp, const char *file_name);

int checkDuplicateID(FILE *fp, const char *id);


int main(int argc, char *argv[])
{
	FILE *fp;						// ��� file processing operation�� C library�� ����� ��

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

    //���ڵ� ���� �κ� ����
        if(strcmp(argv[1], "-i") == 0 ){

        if(argc != 10){
            fprintf(stderr, "need 7 fields of a record.\n");
            exit(1);
        }


        STUDENT s; //��ü ����
        for(int i=3; i<10 ; i++) {

            char field[strlen(argv[i]) + 1];
            char value[strlen(argv[i]) + 1];

            // argv[i] ���� �����Ͽ� field�� value�� �и�
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

        //insert() ���� ��,,, ���ڵ� ���Ͽ� �ش� �й��� �̹� ���� �ϴ��� �ƴ��� �˻�.
        //�����Ѵٸ�...���� ó��

        if (checkDuplicateID(fp, s.id)) {
                fprintf(stderr, "Error: A record with ID %s already exists.\n", s.id);
                exit(1);
        }

        insert(fp, &s);

    }

    //���ڵ� �� �˻� ����
    else if(strcmp(argv[1], "-s") ==0 ) {

        if(argc!=4) {
            fprintf(stderr, "need only 1 field as a key.\n");
            exit(1);
        }

        char field[strlen(argv[3]) + 1];
        char value[strlen(argv[3]) + 1];

        // argv[i] ���� �����Ͽ� field�� value�� �и�
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
    //ó�� ���ڵ� ���� ���� ��. ����� ����.
    fp = fopen(file_name, "w+b");
    if (fp == NULL) {
        perror("make_record() : Error creating file");
        exit(1);
    }

    //headerbuf�� total_pages ������.
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

    //Page�� header area�� ù 2 byte�� ����Ǿ��ִ� #records ����.
    unsigned short total_records;
    memcpy(&total_records, pagebuf, sizeof(total_records));

    //��ü���ڵ� ������ ū recordnum�� ������ error
    if(recordnum >= total_records){
        fprintf(stderr, "getRecFromPagebuf() : recordnum is bigger than total record number\n");
        return 0;
    }
    if(recordnum > 28 || recordnum < 0) {
        fprintf(stderr, "getRecFromPagebuf() : recordnum is out of valid range.\n");
        return 0;
    }


    if(recordnum == 0){ //0��° ���ڵ带 �о��.
        unsigned short offset;
        memcpy(&offset, pagebuf+8, sizeof(offset));
        memcpy(recordbuf, pagebuf+PAGE_HEADER_SIZE, offset+1);

        return 1;
    }
    else{ // recordnum��° ���ڵ带 �о��..
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
    //recordbuf�� pagebuf�� ����ߵ�.

    unsigned short total_records=0;
    memcpy(&total_records, pagebuf, sizeof(total_records));

    unsigned short free_space;
    memcpy(&free_space, pagebuf+2, sizeof(free_space));

    //������ ���� �����ִ� ������ ���ڵ� ���� offset�� ���´�.
    unsigned short offset;
    if(total_records == 0){//ó�� ���� ���
        offset = 0;
        memcpy(pagebuf+PAGE_HEADER_SIZE, recordbuf, strlen(recordbuf));
    }
    else {
        memcpy(&offset, pagebuf + 8 + (total_records - 1) * 2, sizeof(offset));
        memcpy(pagebuf+PAGE_HEADER_SIZE+offset+1, recordbuf, strlen(recordbuf));
    }

    //�������Ƿ� total_record + 1 ����ߵ�.
    total_records++;
    memcpy(pagebuf, &total_records, sizeof(total_records));


    //���Ӱ� ���� ���ڵ��� ������ offset�� �������ߵ�.
    unsigned short new_offset;
    if(offset ==0) {
        new_offset = offset + strlen(recordbuf) - 1;
    }
    else{
        new_offset = offset+strlen(recordbuf);
    }

    memcpy(pagebuf + 8 + (total_records-1) * 2, &new_offset, sizeof(new_offset));

    //free_space �� update
    free_space = PAGE_SIZE-PAGE_HEADER_SIZE - new_offset-1;
    memcpy(pagebuf+2, &free_space, sizeof(free_space));

    //�׽�Ʈ �ڵ�
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
    //���ڵ� ���Ͽ��� field�� Ű��(keyval)�� ���� ���ڵ带 �˻�
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
                        if (found_count < 1000) { // found_records �迭�� ũ�� ���� üũ
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

    //ù ��° ���ڵ� ������ ��� �о�� �������� ������,,,
    //ó�� insert�Ҷ�, �������� ���� �����Ƿ� readPage() ����..
    if(total_pages > 0) {
        last_page_num = total_pages - 1;
        if (!readPage(fp, pagebuf, last_page_num)) { //pagebuf�� �����´�.
            fprintf(stderr, "insert() : Failed to read last page\n");
            return;
        }
        memcpy(&free_space, pagebuf+2, sizeof(free_space));
        memcpy(&total_records, pagebuf, sizeof(total_records));
    }
    else if(total_pages == 0){
        // ���ο� �������� �����ϴ� ��� �������� �ʱ�ȭ
        memset(pagebuf, 0xFF, PAGE_SIZE); // empty pagebuf�� �������

        memcpy(pagebuf, &total_records, sizeof(total_records));

        memcpy(pagebuf+2, &free_space, sizeof(free_space));
        total_pages = 1;
        last_page_num = 0;
    }



    //�ش� �������� �� �� ���� ���. ���� �������� ����ϴ� ���
    if(free_space <  strlen(recordbuf) ){
        total_pages++;
        last_page_num = total_pages - 1;
        memset(pagebuf, 0xFF, PAGE_SIZE); //empty pagebuf�� �������.
        total_records = 0;
        memcpy(pagebuf, &total_records, sizeof(total_records));
        free_space=PAGE_SIZE-PAGE_HEADER_SIZE;
        memcpy(pagebuf+2, &free_space, sizeof(free_space));

    }

    writeRecToPagebuf(pagebuf, recordbuf);
    writePage(fp, pagebuf, last_page_num);

    // ��� ������Ʈ
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
    //header�� �� �� 2B�� ��ü ���������� ����Ǿ� ����.

    //��� �о����
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

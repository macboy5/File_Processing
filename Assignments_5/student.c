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
// �翬��, ���� ���ڵ� ����Ʈ���� 'right size'�� �����ϴ� ���� ���ڵ尡 �����ϸ� ���⿡ ���ο� ���ڵ带 �����ؾ� �Ѵ�.
// ���� ���� ���ڵ� ����Ʈ�� ������ �ʿ��ϴ�.
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

//
// '�й�' Ű���� ���� �л� ���ڵ带 �����Ѵ�. �̶� ���� ���ڵ� ����Ʈ�� ���� �����ϵ��� �����Ǿ�� �Ѵ�.
//
void delete(FILE *fp, char *keyval);

// 
// '�й�' Ű���� ���� �л� ���ڵ带 �˻��Ͽ� �ش� ���ڵ带 recordbuf�� �����Ѵ�.
// �׸��� �ش� ���ڵ��� ������ ��ȣ�� ���ڵ� ��ȣ�� �����Ѵ�. �̰� �� ���� �ʿ��� ������,
// ���ο� ���� ���ڵ尡 ����� ���� ���ڵ� ����Ʈ�� ������Ʈ�ؾ� �ϰ� ���������� ���� �����
// pagenum�� recnum�� ���� ���� ���ο� ���� ���ڵ��� pagenum�� recnum�� �����ؾ� �ϱ� �����̴�.
// �� �Լ��� ���� delete() �Լ����� ȣ���ؼ� ����Ѵ�.
// �˻� ��� ���ڵ尡 �����ϸ� '1', �׷��� ������ '0'�� �����Ѵ�.
//
int searchByID(FILE *fp, char *keyval, char *recordbuf, int *pagenum, int *recordnum);

void make_record_file(FILE *fp, const char *file_name);

int ID_duplication_test(FILE *fp, const char *id);


int main(int argc, char *argv[])
{
    FILE *fp;						// ���� ó�� �۾��� ���� C ���̺귯�� ���� ������

    fp = fopen(argv[2], "r+b");
    if(fp == NULL){
        make_record_file(fp, argv[2]);
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

            // field�� value�� �ʱ�ȭ
            memset(field, 0, sizeof(field));
            memset(value, 0, sizeof(value));

            // argv[i] ���� �����Ͽ� field�� value�� �и�
            int scanned = sscanf(argv[i], "%[^=]=%[^\n]", field, value);
//            printf("Field: %s, Value: %s\n", field, value);

            // value�� �� ���ڿ����� Ȯ���Ͽ� ó��
            if (scanned != 2 || strlen(value) == 0) {
                fprintf(stderr, "Error: Invalid format or missing value for argument : %s\n", argv[i]);
                exit(1);
            }

            //value�� ���̰� ������ ����� �� ó��
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

        //insert() ���� ��,,, ���ڵ� ���Ͽ� �ش� �й��� �̹� ���� �ϴ��� �ƴ��� �˻�.
        //�����Ѵٸ�...���� ó��

        if (ID_duplication_test(fp, s.id)) {
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

        // field�� value�� �ʱ�ȭ
        memset(field, 0, sizeof(field));
        memset(value, 0, sizeof(value));


//      // argv[i] ���� �����Ͽ� field�� value�� �и�
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

        // field�� value�� �ʱ�ȭ
        memset(ID, 0, sizeof(ID));
        memset(value, 0, sizeof(value));


//      // argv[i] ���� �����Ͽ� field�� value�� �и�
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
    //ó�� ���ڵ� ���� ���� ��. ����� ����.
    fp = fopen(file_name, "w+b");
    if (fp == NULL) {
        perror("make_record_file() : Error creating file");
        exit(1);
    }

    char headerbuf[FILE_HEADER_SIZE];
    memset(headerbuf, 0xFF, FILE_HEADER_SIZE);

    //headerbuf�� total_pages ������.
    unsigned short total_pages = 0; // including header page
    memcpy(headerbuf, &total_pages, sizeof(total_pages));

    //���ڵ� ������ ��������� reserved space�� 2B, 2B�� page_num = -1, record_num = -1�� �ʱ�ȭ
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
    else{  // recordnum��° ���ڵ带 �о��.
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
    //pagebuf�� �ҷ��ͼ�...�پ��� �˻縦 ����..
    //recordbuf�� pagebuf�� ����ߵ�.

    unsigned short total_records=0;
    memcpy(&total_records, pagebuf, sizeof(total_records));

    unsigned short free_space;
    memcpy(&free_space, pagebuf+2, sizeof(free_space));

    int no_fit = 1;

    //������ ���� �����ִ� ������ ���ڵ� ���� offset�� ���´�.
    unsigned short offset;
    if(total_records == 0){//ó�� ���� ���
        offset = 0;
        memcpy(pagebuf+PAGE_HEADER_SIZE, recordbuf, strlen(recordbuf));
    }
    else {
        //first-fit ������ ���. ���࿡ ���� �� �ִ� ���� ������ �ǵڿ� append.
        for (int i = 0; i < total_records; i++) {
            //'*' ��ŷ�Ǿ��ִ��� �˻�.
            int del_marker;
            unsigned short offset_curr;
            if (i == 0) {
                memcpy(&del_marker, pagebuf + 8, 1);
            }else {
                memcpy(&offset, pagebuf + 8 + 2 * (i - 1), 2);
                memcpy(&del_marker, pagebuf + PAGE_HEADER_SIZE + offset + 1, 1);
            }

            //���̿� ���� ��..
            if (del_marker == '*' && i <total_records-1) {
                if(i == 0 ){
                    memcpy(&offset_curr, pagebuf + 8 , 2);
                    //���� �� �ִ� ���
                    if( strlen(recordbuf) <= offset_curr+1){
                        no_fit=0;
                        memcpy(pagebuf + PAGE_HEADER_SIZE, recordbuf, strlen(recordbuf));
                    }
                }
                else {
                    memcpy(&offset, pagebuf + 8 + 2 * (i - 1), 2);
                    memcpy(&offset_curr, pagebuf + 8 + 2 * i, 2);
                    //���� �� �ִ� ���..
                    if (strlen(recordbuf) <= offset_curr - offset-1) {
                        no_fit = 0;
                        memcpy(pagebuf + PAGE_HEADER_SIZE + offset_curr + 1, recordbuf, strlen(recordbuf));
                    }

                }
            }

            //�� ������ record�� ������ ���..
            if(del_marker == '*' && i == total_records-1) {
                //���� �� �ִ� ���.
                if (offset + strlen(recordbuf) <= PAGE_SIZE - PAGE_HEADER_SIZE) {
                    no_fit = 0;
                    memcpy(pagebuf + PAGE_HEADER_SIZE + offset_curr + 1, recordbuf, strlen(recordbuf));
                }
                else{
                    //���� �� �������� ������ϴ� ���.
                    //empty pagebuf�� ����� �ű⿡ �Ἥ �Ѱ������.
                    no_fit = 1;

                    memset(pagebuf, 0xFF, PAGE_SIZE); //empty pagebuf�� �������.

                    //recordbuf�� pagebuf�� ����.
                    memcpy(pagebuf + PAGE_HEADER_SIZE, recordbuf, strlen(recordbuf));

                    //��� ������Ʈ
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

    //del�� ���� ���� ��� --> �� �ڿ� append.. �� �ڿ� �� �� ���ٸ� ���� �������� �Ѱ������.
    if (no_fit) {
        if(strlen(recordbuf) > free_space){
            //���� �� �������� ������ϴ� ���.
            //empty pagebuf�� ����� �ű⿡ �Ἥ �Ѱ������.

            memset(pagebuf, 0xFF, PAGE_SIZE); //empty pagebuf�� �������.

            //recordbuf�� pagebuf�� ����.
            memcpy(pagebuf + PAGE_HEADER_SIZE, recordbuf, strlen(recordbuf));

            //��� ������Ʈ
            total_records = 1;
            memcpy(pagebuf, &total_records, sizeof(total_records));

            free_space=PAGE_SIZE-PAGE_HEADER_SIZE- strlen(recordbuf);
            memcpy(pagebuf+2, &free_space, sizeof(free_space));

            unsigned short offset_by_len = strlen(recordbuf)-1;
            memcpy(pagebuf+8, &offset_by_len, sizeof(offset_by_len));
        }
        else{
            //�� �ڿ� append ������ ���
            // total_record + 1 ����ߵ�.
            total_records++;
            memcpy(pagebuf, &total_records, sizeof(total_records));


            //���Ӱ� ���� ���ڵ��� ������ offset�� �������ߵ�.
            unsigned short new_offset;
            if (offset == 0) {
                new_offset = offset + strlen(recordbuf) - 1;
            } else {
                new_offset = offset + strlen(recordbuf);
            }

            memcpy(pagebuf + 8 + (total_records - 1) * 2, &new_offset, sizeof(new_offset));

            //free_space �� update
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
                return 1; // �ߺ� ID, �̹� ID�� ������ ?? 1 return.
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
        if (!readPage(fp, pagebuf, last_page_num)) {  //������ �������� pagebuf�� �����´�.
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
        // ���ο� �������� �����ϴ� ��� �������� �ʱ�ȭ
        memset(pagebuf, 0xFF, PAGE_SIZE); // empty pagebuf�� �������

        memcpy(pagebuf, &total_records, sizeof(total_records));

        memcpy(pagebuf+2, &free_space, sizeof(free_space));
        total_pages = 1;
        last_page_num = 0;

        writeRecToPagebuf(pagebuf, recordbuf);
        writePage(fp, pagebuf, last_page_num);
    }



//    //�ش� �������� �� �� ���� ���. ���� �������� ����ϴ� ���..���� �������� ����.
//    if(free_space <  strlen(recordbuf) ){
//        total_pages++;
//        last_page_num = total_pages - 1;
//        memset(pagebuf, 0xFF, PAGE_SIZE); //empty pagebuf�� �������.
//        total_records = 0;
//        memcpy(pagebuf, &total_records, sizeof(total_records));
//        free_space=PAGE_SIZE-PAGE_HEADER_SIZE;
//        memcpy(pagebuf+2, &free_space, sizeof(free_space));
//
//    }

//    writeRecToPagebuf(pagebuf, recordbuf);
//    writePage(fp, pagebuf, last_page_num);

//    // ��� ������Ʈ : total_record == 1 �ΰ��.. �� ������ ó���� ���� ��� Ȥ�� ���� �������� �� ���..
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

void delete(FILE *fp, char *keyval){
    char recordbuf[MAX_RECORD_SIZE];

    //���ڵ� ���Ͽ��� field�� Ű��(keyval)�� ���� ���ڵ带 �˻� ... (field, keyval)
    char headerbuf[FILE_HEADER_SIZE];
    if (!readFileHeader(fp, headerbuf)) {
        fprintf(stderr, "search() : Failed to read file header\n");
        return;
    }

    //���ڵ� ������ reserved area�� �����ִ� ������ ���� ��ġ ��������.
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

            //��ġ�ϴ� ID�� ���ڵ� ������ ã�� ���.. ���� �۾� ����
            //��ġ�ϴ� ID�� �ִ� recordbuf�� �����Դ�..
            if(searchByID(fp, keyval, recordbuf, &i, &j)){

                //recordbuf ������ *PagenumRecordnum���� �����
                //pagenum�� ������ �����Ǿ��� ������ ��ȣ, recordnum�� ������ �����Ǿ��� ���ڵ� ��ȣ..
                //���ڵ� ���� ����� ����Ǿ� �ִ� ���� �����ͼ� ������Ʈ���ְ�
                //���ڵ� ������ ����� ���� �����Ǵ� ��������ȣ, ���ڵ� ��ȣ�� ������Ʈ.
                memset(recordbuf, 0xFF, strlen(recordbuf));

                memset(recordbuf, '*', 1);
                memset(recordbuf+1, pagenum, 2);
                memset(recordbuf+3, recordnum, 2);

                memset(headerbuf+2, i, 2);
                memset(headerbuf+4, j ,2);
                writeFileHeader(fp, headerbuf);

                //��� recordbuf�� ���� ��ġ�� ã�ư��� ������Ʈ
                //pagebuf���� offset�� ���ؼ� �װ��� update..

                if(j == 0){ //ù��° ���ڵ尡 �����Ǵ� ���
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

    //pagenum�� recordnum���� ���� �˻縦 ����.
    char pagebuf[PAGE_SIZE];
    readPage(fp, pagebuf, *pagenum);

    char record_buf[MAX_RECORD_SIZE];
    getRecFromPagebuf(pagebuf, recordbuf, *recordnum);

    STUDENT s;

    unpack(record_buf, &s);

    if(s.id == keyval){
        //ã������ �ش� ���ڵ带 recordbuf�� ����.
        memcpy(recordbuf, record_buf, sizeof(record_buf));

        printSearchResult(&s, 1);

        return 1;
    }


    //��ã�� ��쿡�� 0�� return.

    return 0;
}



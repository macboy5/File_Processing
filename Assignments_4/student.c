#include <stdio.h>		// \C7ʿ\E4\C7\D1 header file \C3߰\A1 \B0\A1\B4\C9
#include "student.h"
#include <string.h>
#include <stdlib.h>

//
// readPage() \C7Լ\F6\B4\C2 \B7\B9\C4ڵ\E5 \C6\C4\C0Ͽ\A1\BC\AD \C1־\EE\C1\F8 \C6\E4\C0\CC\C1\F6\B9\F8ȣ pagenum(=0, 1, 2, ...)\BF\A1 \C7ش\E7\C7ϴ\C2 page\B8\A6
// \C0о pagebuf\B0\A1 \B0\A1\B8\AEŰ\B4\C2 \B0\F7\BF\A1 \C0\FA\C0\E5\C7ϴ\C2 \BF\AA\C7\D2\C0\BB \BC\F6\C7\E0\C7Ѵ\D9. \C1\A4\BB\F3\C0\FB\C0\CE \BC\F6\C7\E0\C0\BB \C7\D1 \B0\E6\BF\EC
// '1'\C0\BB, \B1׷\B8\C1\F6 \BE\CA\C0\BA \B0\E6\BF\EC\B4\C2 '0'\C0\BB \B8\AE\C5\CF\C7Ѵ\D9.
// getRecFromPagebuf() \C7Լ\F6\B4\C2 readPage()\B8\A6 \C5\EB\C7\D8 \C0о\EE\BF\C2 page\BF\A1\BC\AD \C1־\EE\C1\F8 \B7\B9\C4ڵ\E5\B9\F8ȣ recordnum(=0, 1, 2, ...)\BF\A1
// \C7ش\E7\C7ϴ\C2 \B7\B9\C4ڵ带 recordbuf\BF\A1 \C0\FC\B4\DE\C7ϴ\C2 \C0\CF\C0\BB \BC\F6\C7\E0\C7Ѵ\D9.
// \B8\B8\BE\E0 page\BF\A1\BC\AD \C0\FC\B4\DE\C7\D2 record\B0\A1 \C1\B8\C0\E7\C7ϸ\E9 '1'\C0\BB, \B1׷\B8\C1\F6 \BE\CA\C0\B8\B8\E9 '0'\C0\BB \B8\AE\C5\CF\C7Ѵ\D9.
// unpack() \C7Լ\F6\B4\C2 recordbuf\BF\A1 \C0\FA\C0\E5\B5Ǿ\EE \C0ִ\C2 record\BF\A1\BC\AD \B0\A2 field\B8\A6 \C3\DF\C3\E2\C7Ͽ\A9 \C7л\FD \B0\B4ü\BF\A1 \C0\FA\C0\E5\C7ϴ\C2 \C0\CF\C0\BB \C7Ѵ\D9.
//
int readPage(FILE *fp, char *pagebuf, int pagenum);
int getRecFromPagebuf(const char *pagebuf, char *recordbuf, int recordnum);
void unpack(const char *recordbuf, STUDENT *s);

//
// \C1־\EE\C1\F8 \C7л\FD \B0\B4ü\B8\A6 \B7\B9\C4ڵ\E5 \C6\C4\C0Ͽ\A1 \C0\FA\C0\E5\C7\D2 \B6\A7 \BB\E7\BF\EB\B5Ǵ\C2 \C7Լ\F6\B5\E9\C0̸\E7, \B1\D7 \BDó\AA\B8\AE\BF\C0\B4\C2 \B4\D9\C0\BD\B0\FA \B0\B0\B4\D9.
// 1. \C7л\FD \B0\B4ü\B8\A6 \BD\C7\C1\A6 \B7\B9\C4ڵ\E5 \C7\FC\C5\C2\C0\C7 recordbuf\BF\A1 \C0\FA\C0\E5\C7Ѵ\D9(pack() \C7Լ\F6 \C0̿\EB).
// 2. \B7\B9\C4ڵ\E5 \C6\C4\C0\CF\C0\C7 \B8Ǹ\B6\C1\F6\B8\B7 page\B8\A6 pagebuf\BF\A1 \C0о\EE\BF´\D9(readPage() \C0̿\EB). \B8\B8\BE\E0 \BB\F5\B7ο\EE \B7\B9\C4ڵ带 \C0\FA\C0\E5\C7\D2 \B0\F8\B0\A3\C0\CC
//    \BA\CE\C1\B7\C7ϸ\E9 pagebuf\BF\A1 empty page\B8\A6 \C7ϳ\AA \BB\FD\BC\BA\C7Ѵ\D9.
// 3. recordbuf\BF\A1 \C0\FA\C0\E5\B5Ǿ\EE \C0ִ\C2 \B7\B9\C4ڵ带 pagebuf\C0\C7 page\BF\A1 \C0\FA\C0\E5\C7Ѵ\D9(writeRecToPagebuf() \C7Լ\F6 \C0̿\EB).
// 4. \BC\F6\C1\A4\B5\C8 page\B8\A6 \B7\B9\C4ڵ\E5 \C6\C4\C0Ͽ\A1 \C0\FA\C0\E5\C7Ѵ\D9(writePage() \C7Լ\F6 \C0̿\EB)
// 
// writePage()\B4\C2 \BC\BA\B0\F8\C0\FB\C0\B8\B7\CE \BC\F6\C7\E0\C7ϸ\E9 '1'\C0\BB, \B1׷\B8\C1\F6 \BE\CA\C0\B8\B8\E9 '0'\C0\BB \B8\AE\C5\CF\C7Ѵ\D9.
//
int writePage(FILE *fp, const char *pagebuf, int pagenum);
void writeRecToPagebuf(char *pagebuf, const char *recordbuf);
void pack(char *recordbuf, const STUDENT *s);

//
// \B7\B9\C4ڵ\E5 \C6\C4\C0Ͽ\A1\BC\AD field\C0\C7 Ű\B0\AA(keyval)\C0\BB \B0\AE\B4\C2 \B7\B9\C4ڵ带 \B0˻\F6\C7ϰ\ED \B1\D7 \B0\E1\B0\FA\B8\A6 \C3\E2\B7\C2\C7Ѵ\D9.
// \B0˻\F6\B5\C8 \B7\B9\C4ڵ带 \C3\E2\B7\C2\C7\D2 \B6\A7 \B9ݵ\E5\BD\C3 printSearchResult() \C7Լ\F6\B8\A6 \BB\E7\BF\EB\C7Ѵ\D9.
//
void search(FILE *fp, FIELD f, char *keyval);
void printSearchResult(const STUDENT *s, int n);

//
// \B7\B9\C4ڵ\E5 \C6\C4\C0Ͽ\A1 \BB\F5\B7ο\EE \C7л\FD \B7\B9\C4ڵ带 \C3߰\A1\C7\D2 \B6\A7 \BB\E7\BF\EB\C7Ѵ\D9. ǥ\C1\D8 \C0Է\C2\C0\B8\B7\CE \B9\DE\C0\BA \C7ʵ尪\B5\E9\C0\BB \C0̿\EB\C7Ͽ\A9
// \C7л\FD \B0\B4ü\B7\CE \B9ٲ\DB \C8\C4 insert() \C7Լ\F6\B8\A6 ȣ\C3\E2\C7Ѵ\D9.
//
void insert(FILE *fp, const STUDENT *s);

//
// \B7\B9\C4ڵ\E5\C0\C7 \C7ʵ\E5\B8\ED\C0\BB enum FIELD Ÿ\C0\D4\C0\C7 \B0\AA\C0\B8\B7\CE \BA\AFȯ\BD\C3\C4\D1 \C1ش\D9.
// \BF\B9\B8\A6 \B5\E9\B8\E9, \BB\E7\BF\EB\C0ڰ\A1 \BC\F6\C7\E0\C7\D1 \B8\ED\B7ɾ\EE\C0\C7 \C0\CE\C0ڷ\CE "NAME"\C0̶\F3\B4\C2 \C7ʵ\E5\B8\ED\C0\BB \BB\E7\BF\EB\C7Ͽ\B4\B4ٸ\E9 
// \C7\C1\B7α׷\A5 \B3\BB\BAο\A1\BC\AD \C0̸\A6 NAME(=1)\C0\B8\B7\CE \BA\AFȯ\C7\D2 \C7ʿ伺\C0\CC \C0\D6\C0\B8\B8\E7, \C0̶\A7 \C0\CC \C7Լ\F6\B8\A6 \C0̿\EB\C7Ѵ\D9.
//
FIELD getFieldID(char *fieldname);

//
// \B7\B9\C4ڵ\E5 \C6\C4\C0\CF\C0\C7 \C7\EC\B4\F5\B8\A6 \C0о\EE \BF\C0\B0ų\AA \BC\F6\C1\A4\C7\D2 \B6\A7 \BB\E7\BF\EB\C7Ѵ\D9.
// \BF\B9\B8\A6 \B5\E9\BE\EE, \BB\F5\B7ο\EE #pages\B8\A6 \BC\F6\C1\A4\C7ϱ\E2 \C0\A7\C7ؼ\AD\B4\C2 \B8\D5\C0\FA readFileHeader()\B8\A6 ȣ\C3\E2\C7Ͽ\A9 \C7\EC\B4\F5\C0\C7 \B3\BB\BF\EB\C0\BB
// headerbuf\BF\A1 \C0\FA\C0\E5\C7\D1 \C8\C4 \BF\A9\B1⿡ #pages\B8\A6 \BC\F6\C1\A4\C7ϰ\ED writeFileHeader()\B8\A6 ȣ\C3\E2\C7Ͽ\A9 \BA\AF\B0\E6\B5\C8 \C7\EC\B4\F5\B8\A6 
// \C0\FA\C0\E5\C7Ѵ\D9. \B5\CE \C7Լ\F6 \B8\F0\B5\CE \BC\BA\B0\F8\C7ϸ\E9 '1'\C0\BB, \B1׷\B8\C1\F6 \BE\CA\C0\B8\B8\E9 '0'\C0\BB \B8\AE\C5\CF\C7Ѵ\D9.
//
int readFileHeader(FILE *fp, char *headerbuf);

int writeFileHeader(FILE *fp, const char *headerbuf);

void make_record_file(FILE *fp, const char *file_name);

int ID_duplication_test(FILE *fp, const char *id);


int main(int argc, char *argv[])
{
	FILE *fp;						// \B8\F0\B5\E7 file processing operation\C0\BA C library\B8\A6 \BB\E7\BF\EB\C7\D2 \B0\CD

    fp = fopen(argv[2], "r+b");
    if(fp == NULL){
        //fclose(fp);
        make_record_file(fp, argv[2]);
        fp = fopen(argv[2], "r+b");
        if(fp == NULL){
            perror("Error opening file");
            exit(1);
        }
    }

    //\B7\B9\C4ڵ\E5 \C0\FA\C0\E5 \BAκ\D0 \B1\B8\C7\F6
        if(strcmp(argv[1], "-i") == 0 ){

        if(argc != 10){
            fprintf(stderr, "need 7 fields of a record.\n");
            exit(1);
        }


        STUDENT s; //\B0\B4ü \BB\FD\BC\BA
        for(int i=3; i<10 ; i++) {

            char field[strlen(argv[i]) + 1];
            char value[strlen(argv[i]) + 1];

            // field\BF\CD value\B8\A6 \C3ʱ\E2ȭ
            memset(field, 0, sizeof(field));
            memset(value, 0, sizeof(value));

            // argv[i] \B0\AA\C0\BB \BA\B9\BB\E7\C7Ͽ\A9 field\BF\CD value\B7\CE \BAи\AE
            int scanned = sscanf(argv[i], "%[^=]=%[^\n]", field, value);
//            printf("Field: %s, Value: %s\n", field, value);

            // value\B0\A1 \BA\F3 \B9\AE\C0ڿ\AD\C0\CE\C1\F6 Ȯ\C0\CE\C7Ͽ\A9 ó\B8\AE
            if (scanned != 2 || strlen(value) == 0) {
                fprintf(stderr, "Error: Invalid format or missing value for argument : %s\n", argv[i]);
                exit(1);
            }

            //value\C0\C7 \B1\E6\C0̰\A1 \B9\FC\C0\A7\B8\A6 \B9\FE\BE\C0\BB \B6\A7 ó\B8\AE
            switch (i) {
                case 3:
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

        //insert() \BD\C7\C7\E0 \C0\FC,,, \B7\B9\C4ڵ\E5 \C6\C4\C0Ͽ\A1 \C7ش\E7 \C7й\F8\C0\CC \C0̹\CC \C1\B8\C0\E7 \C7ϴ\C2\C1\F6 \BEƴ\D1\C1\F6 \B0˻\E7.
        //\C1\B8\C0\E7\C7Ѵٸ\E9...\BF\A1\B7\AF ó\B8\AE

        if (ID_duplication_test(fp, s.id)) {
                fprintf(stderr, "Error: A record with ID %s already exists.\n", s.id);
                exit(1);
        }

        insert(fp, &s);

    }

    //\B7\B9\C4ڵ\E5 \B3\BB \B0˻\F6 \B1\B8\C7\F6
    else if(strcmp(argv[1], "-s") ==0 ) {

        if(argc!=4) {
            fprintf(stderr, "need only 1 field as a key.\n");
            exit(1);
        }



        char field[strlen(argv[3]) + 1];
        char value[strlen(argv[3]) + 1];

            // field\BF\CD value\B8\A6 \C3ʱ\E2ȭ
            memset(field, 0, sizeof(field));
            memset(value, 0, sizeof(value));


//        // argv[i] \B0\AA\C0\BB \BA\B9\BB\E7\C7Ͽ\A9 field\BF\CD value\B7\CE \BAи\AE
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

    else{
        fprintf(stderr, "only -i or -s is available for first the argument\n");
        exit(1);
    }

    fclose(fp);

	return 1;
}





void make_record_file(FILE *fp, const char *file_name){
    //ó\C0\BD \B7\B9\C4ڵ\E5 \C6\C4\C0\CF \BB\FD\BC\BA \BD\C3. \C7\EC\B4\F5\B8\B8 \BB\FD\BC\BA.
    fp = fopen(file_name, "w+b");
    if (fp == NULL) {
        perror("make_record_file() : Error creating file");
        exit(1);
    }

    //headerbuf\BF\A1 total_pages \C0\FB\BE\EE\C1\DC.
    char headerbuf[FILE_HEADER_SIZE];
    memset(headerbuf, 0xFF, FILE_HEADER_SIZE);

    unsigned short total_pages = 0; // including header page
    memcpy(headerbuf, &total_pages, sizeof(total_pages));

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

    //Page\C0\C7 header area\C0\C7 ù 2 byte\BF\A1 \C0\FA\C0\E5\B5Ǿ\EE\C0ִ\C2 #records \C1\A4\BA\B8.
    unsigned short total_records;
    memcpy(&total_records, pagebuf, sizeof(total_records));

    //\C0\FCü\B7\B9\C4ڵ\E5 \BC\F6\BA\B8\B4\D9 ū recordnum\C0\CC \B5\E9\BE\EE\BF\C0\B8\E9 error
    if(recordnum >= total_records){
        fprintf(stderr, "getRecFromPagebuf() : recordnum is bigger than total record number\n");
        return 0;
    }
    if(recordnum > 28 || recordnum < 0) {
        fprintf(stderr, "getRecFromPagebuf() : recordnum is out of valid range.\n");
        return 0;
    }


    if(recordnum == 0){ //0\B9\F8° \B7\B9\C4ڵ带 \C0о\EE\B6\F3.
        unsigned short offset;
        memcpy(&offset, pagebuf+8, sizeof(offset));
        memcpy(recordbuf, pagebuf+PAGE_HEADER_SIZE, offset+1);

        return 1;
    }
    else{ // recordnum\B9\F8° \B7\B9\C4ڵ带 \C0о\EE\B6\F3..
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
    //recordbuf\B8\A6 pagebuf\BF\A1 \C0\FB\BE\EE\BEߵ\CA.

    unsigned short total_records=0;
    memcpy(&total_records, pagebuf, sizeof(total_records));

    unsigned short free_space;
    memcpy(&free_space, pagebuf+2, sizeof(free_space));

    //\C6\E4\C0\CC\C1\F6 \B3\BB\BF\A1 \C0\FB\C7\F4\C0ִ\F8 \B8\B6\C1\F6\B8\B7 \B7\B9\C4ڵ\E5 \B4\D9\C0\BD offset\BF\A1 \C0\FB\B4´\D9.
    unsigned short offset;
    if(total_records == 0){//ó\C0\BD \C0\FB\B4\C2 \B0\E6\BF\EC
        offset = 0;
        memcpy(pagebuf+PAGE_HEADER_SIZE, recordbuf, strlen(recordbuf));
    }
    else {
        memcpy(&offset, pagebuf + 8 + (total_records - 1) * 2, sizeof(offset));
        memcpy(pagebuf+PAGE_HEADER_SIZE+offset+1, recordbuf, strlen(recordbuf));
    }

    //\C0\FB\BE\FA\C0\B8\B9Ƿ\CE total_record + 1 \C7\D8\C1\E0\BEߵ\CA.
    total_records++;
    memcpy(pagebuf, &total_records, sizeof(total_records));


    //\BB\F5\B7Ӱ\D4 \C0\FB\C0\BA \B7\B9\C4ڵ\E5\C0\C7 \B8\B6\C1\F6\B8\B7 offset\C0\BB \B1\E2\B7\CF\C7\D8\C1\E0\BEߵ\CA.
    unsigned short new_offset;
    if(offset ==0) {
        new_offset = offset + strlen(recordbuf) - 1;
    }
    else{
        new_offset = offset+strlen(recordbuf);
    }

    memcpy(pagebuf + 8 + (total_records-1) * 2, &new_offset, sizeof(new_offset));

    //free_space \B0\AA update
    free_space = PAGE_SIZE-PAGE_HEADER_SIZE - new_offset-1;
    memcpy(pagebuf+2, &free_space, sizeof(free_space));

    //\C5׽\BAƮ \C4ڵ\E5
//    printf("writeRecToPagebuf() test\n");
//    for(int i=0; i<PAGE_SIZE; i++){
//        printf("%02x ", pagebuf[i]);
//    }
//    printf("\n");

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
    //\B7\B9\C4ڵ\E5 \C6\C4\C0Ͽ\A1\BC\AD field\C0\C7 Ű\B0\AA(keyval)\C0\BB \B0\AE\B4\C2 \B7\B9\C4ڵ带 \B0˻\F6
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
                        if (found_count < 1000) { // found_records \B9迭\C0\C7 ũ\B1\E2 \C1\A6\C7\D1 üũ
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
                return 1; // \C1ߺ\B9 ID, \C0̹\CC ID\B0\A1 \C1\B8\C0\E7\C7\D2 \8B\9A 1 return.
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

    //ù \B9\F8° \B7\B9\C4ڵ\E5 \C0\FA\C0\E5\C0\C7 \B0\E6\BF\EC \C0о\EE\BF\C3 \C6\E4\C0\CC\C1\F6\B0\A1 \BE\F8\C0\B8\B4\CF,,,
    //ó\C0\BD insert\C7Ҷ\A7, \C6\E4\C0\CC\C1\F6\B0\A1 \BE\C6\C1\F7 \BE\F8\C0\B8\B9Ƿ\CE readPage() \BB\FD\B7\AB..
    if(total_pages > 0) {
        last_page_num = total_pages - 1;
        if (!readPage(fp, pagebuf, last_page_num)) { //pagebuf\B8\A6 \B0\A1\C1\AE\BF´\D9.
            fprintf(stderr, "insert() : Failed to read last page\n");
            return;
        }
        memcpy(&free_space, pagebuf+2, sizeof(free_space));
        memcpy(&total_records, pagebuf, sizeof(total_records));
    }
    else if(total_pages == 0){
        // \BB\F5\B7ο\EE \C6\E4\C0\CC\C1\F6\B8\A6 \BB\FD\BC\BA\C7ϴ\C2 \B0\E6\BF\EC \C6\E4\C0\CC\C1\F6\B8\A6 \C3ʱ\E2ȭ
        memset(pagebuf, 0xFF, PAGE_SIZE); // empty pagebuf\B7\CE \B8\B8\B5\E9\BE\EE\C1\DC

        memcpy(pagebuf, &total_records, sizeof(total_records));

        memcpy(pagebuf+2, &free_space, sizeof(free_space));
        total_pages = 1;
        last_page_num = 0;
    }



    //\C7ش\E7 \C6\E4\C0\CC\C1\F6\BF\A1 \BE\B5 \BC\F6 \BE\F8\B4\C2 \B0\E6\BF\EC. \B4\D9\C0\BD \C6\E4\C0\CC\C1\F6\BF\A1 \BD\E1\BE\DF\C7ϴ\C2 \B0\E6\BF\EC
    if(free_space <  strlen(recordbuf) ){
        total_pages++;
        last_page_num = total_pages - 1;
        memset(pagebuf, 0xFF, PAGE_SIZE); //empty pagebuf\B7\CE \B8\B8\B5\E9\BE\EE\C1\DC.
        total_records = 0;
        memcpy(pagebuf, &total_records, sizeof(total_records));
        free_space=PAGE_SIZE-PAGE_HEADER_SIZE;
        memcpy(pagebuf+2, &free_space, sizeof(free_space));

    }

    writeRecToPagebuf(pagebuf, recordbuf);
    writePage(fp, pagebuf, last_page_num);

    // \C7\EC\B4\F5 \BE\F7\B5\A5\C0\CCƮ
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
    //header\C0\C7 \B8\C7 \BE\D5 2B\BF\A1 \C0\FCü \C6\E4\C0\CC\C1\F6\BC\F6\B0\A1 \C0\FA\C0\E5\B5Ǿ\EE \C0\D6\C0\BD.

    //\C7\EC\B4\F5 \C0о\EE\BF\C0\B1\E2
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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    
    //예외처리
    if(argc !=4){
        fprintf(stderr, "Input Error : write <offset> <bytes> <filename>.\n");
        exit(1);
    }

    int offset = atoi(argv[1]);
    int bytes = atoi(argv[2]);
    int fd = open(argv[3], O_RDONLY );


    if(fd < 0){
        fprintf(stderr, "%s is not exist.\n", argv[3]);
        exit(1);
    }
    int file_size = lseek(fd, 0, SEEK_END);

    if(file_size == -1) {
        fprintf(stderr,"lseek error().\n");
        exit(1);
    }
    //오프셋이 파일 크기의 범위를 벗어날 경우
    if(offset > file_size){
        fprintf(stderr, "Out of bounds : Offset is bigger than file size.\n");
        exit(1);
    }

    char* buffer = NULL;
    //바이트 수 음수
    if(bytes < 0){
        //bytes의 절댓값이 offset보다 더 크거나 같을 때 0부터 offset-1까지 읽고 출력
        if(abs(bytes) >= offset){
            //포인터 위치 처음으로 이동.
            lseek(fd, 0, SEEK_SET);

            //offset크기만큼 동적할당
            buffer = (char*)(malloc(offset));
            read(fd, buffer, offset);
            printf("%s", buffer);
        /*    write(fd, buffer, offset);*/
         /*   fwrite(buffer, 1, offset, stdout);*/
        }
        //bytes의 절댓값이 offset보다 작은 경우.. offset-bytes부터 offset-1까지 읽고 출력.
        else{
            lseek(fd, offset-abs(bytes), SEEK_SET);
            buffer = (char*)(malloc)((abs(bytes)));
            read(fd, buffer, abs(bytes));
            printf("%s", buffer);
          /*  write(fd, buffer, abs(bytes));*/
          /*  fwrite(buffer, 1, abs(bytes), stdout);*/
        }
    }

    //바이트 수 양수
    else if(bytes > 0 ){
        //offset+bytes가 file_size넘어가는경우
        if(offset+bytes >=file_size){
            lseek(fd, offset+1, SEEK_SET);
            buffer = (char*)(malloc)(file_size-offset);
            read(fd, buffer, file_size-offset);
            printf("%s", buffer);
         /*   write(fd, buffer, file_size-offset);*/
       /*     fwrite(buffer, 1, file_size - offset, stdout);*/
        }
        else{
            lseek(fd, offset+1, SEEK_SET);
            buffer = (char*)(malloc)((bytes));
            read(fd, buffer, bytes);
            printf("%s", buffer);
     /*       write(fd, buffer, bytes);*/
         /*   fwrite(buffer, 1, bytes, stdout);*/
        }
    }

    //바이트 수 0
    else{
        exit(1);
    }

    free(buffer);
    close(fd);
    exit(0);
}
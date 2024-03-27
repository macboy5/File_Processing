#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]){

    if(argc != 3){
        fprintf(stderr, "Input Error : Write <Original File name> <Copy target file name>.\n");
        exit(1);
    }

    int origin_fd = open(argv[1], O_RDONLY);
    int copied_fd = open(argv[2], O_CREAT|O_WRONLY);

    if(origin_fd < 0){
        fprintf(stderr, "%s is not exist.\n", argv[1]);
        exit(1);
    }

    if(copied_fd < 0){
        fprintf(stderr, "%s is not exist.\n", argv[2]);
        exit(1);
    }

    //포인터 위치 맨앞으로 초기화
    lseek(origin_fd, 0, SEEK_SET);

    char buffer[10];

    while(1){
        int len = read(origin_fd, buffer, 10);
        write(copied_fd, buffer, len);
        //마지막 읽었을 때 길이가 10 미만이면 break;
        if(len < 10) break;

    }

    close(copied_fd);
    close(origin_fd);
  
  exit(0);
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[]){

    if(argc != 4){
        fprintf(stderr, "Input Error : Write <Offset> <Data> <File name>.\n");
        exit(1);
    }
    

    int offset = atoi(argv[1]);
    int fd = open(argv[3], O_RDWR);
    
    if(fd < 0){
        fprintf(stderr, "%s is not exist.\n", argv[3]);
        exit(3);
    }

    int file_size = lseek(fd, 0, SEEK_END);
    //오프셋 범위 예외처리
    if(offset > file_size){
        fprintf(stderr, "Out of bounds : Offset is bigger than file size.\n");
        exit(1);
    }

    lseek(fd, offset, SEEK_SET);
    write(fd, argv[2], strlen(argv[2]));

    close(fd);

    exit(0);
}

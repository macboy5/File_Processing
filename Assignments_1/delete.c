#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[]) {

    if(argc != 4) {
        fprintf(stderr, "Input Error : Write <Offset> <Bytes> <File Name>.\n");
        exit(1);
    }

    int offset = atoi(argv[1]);
    int bytes = atoi(argv[2]);
    int fd = open(argv[3], O_RDWR);

    if(fd < 0) {
        fprintf(stderr, "%s does not exist.\n", argv[3]);
        exit(1);
    }

    int end = lseek(fd, 0, SEEK_END);

    if(end < 0) {
        fprintf(stderr, "lseek error().\n");
        close(fd);
        exit(1);
    }

    if(offset > end) {
        fprintf(stderr, "Out of bounds: Offset is bigger than file size.\n");
        close(fd);
        exit(1);
    }

    //바이트 수 음수인 경우
    if(bytes < 0) {
        //바이트수의 절댓값이 offset보다 크거나 같은 때, 즉 오프셋 기준 왼쪽 데이터 전부 삭제되는 경우.
        if(abs(bytes) >= offset) {
            //오프셋 포함 오른쪽 데이터 전부 버퍼에 저장.
            char* buffer = (char*)malloc(end - offset );
            lseek(fd, offset, SEEK_SET);
            read(fd, buffer, end - offset );

            //파일 크기 0으로 초기화.
            ftruncate(fd, 0);

            //buffer에 저장된 데이터를 맨 앞에 write()
            lseek(fd, 0, SEEK_SET);
            write(fd, buffer, end - offset );

            free(buffer);

        } else {
            //바이트수의 절댓값이 offset보다 작을 때

            //buffer1에 오프셋 오른쪽 데이터 저장.
            char* buffer1 = (char*)malloc(end - offset );
            lseek(fd, offset, SEEK_SET);
            read(fd, buffer1, end - offset );

            //buffer2에 오프셋 왼쪽 편 데이터 중 삭제되고 남을 데이터 저장.
            char* buffer2 = (char*)malloc(offset - abs(bytes) );
            lseek(fd, 0, SEEK_SET);
            read(fd, buffer2, offset - abs(bytes) );

            //파일 크기 0으로 초기화.
            ftruncate(fd, 0);

            //포인터 맨앞으로 옮기고 buffer2에 있는 내용 붙여 넣기.
            lseek(fd, 0, SEEK_SET);
            write(fd, buffer2, offset - abs(bytes) );

            //포인터 삭제 시작 위치로 옮기고 buffer1에 있는 내용 붙여 넣기.
            lseek(fd, offset - abs(bytes), SEEK_SET);
            write(fd, buffer1, end - offset );

            free(buffer1);
            free(buffer2);

        }
    }
    //바이트 수 양수
    else if(bytes > 0) {
        //바이트 수가 오프셋 오른쪽의 데이터 수보다 크거나 같은 경우, 즉 오프셋 오른쪽 전부 다 삭제하는 경우.
        if(bytes >= end - offset) {
            //버퍼에 오프셋 포함 왼쪽 데이터 저장.
            char* buffer = (char*)malloc(offset +1);
            lseek(fd, 0, SEEK_SET);
            read(fd, buffer, offset +1);

            //파일 크기 0으로 초기화.
            ftruncate(fd, 0);

            //포인터 맨앞으로 가져오고 버퍼에 있는 내용 붙여넣기.
            lseek(fd, 0, SEEK_SET);
            write(fd, buffer, offset +1);

            free(buffer);
        } else {
            //바이트수가 오프셋 오른쪽 데이터 수보다는 작은 경우

            //버퍼에 삭제하고 남을 데이터 저장.
            char* buffer = (char*)malloc(end - offset - bytes + 1);
            lseek(fd, offset + bytes+1, SEEK_SET);
            read(fd, buffer, end - offset - bytes);

            //offset+1 위치로 포인터 이동 후 붙여 넣기.
            lseek(fd, offset+1, SEEK_SET);
            write(fd, buffer, end - offset - bytes);

            //뒤에 남을 잉여 데이터를 지워주기 위해 파일크기를 end - bytes로 조정.
            ftruncate(fd, end - bytes);

            free(buffer);
        }
    } else {
        exit(1);
    }

    close(fd);
    exit(0);
}

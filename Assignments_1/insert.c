#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Input Error: Write <Offset> <Data> <File Name>.\n");
        exit(1);
    }

    int offset = atoi(argv[1]);
    int fd = open(argv[3], O_RDWR);

    if (fd < 0) {
        fprintf(stderr, "%s does not exist.\n", argv[3]);
        exit(1);
    }

    int end = lseek(fd, 0, SEEK_END);

    // 오프셋이 파일의 끝에 있으면 append 처리
    if (offset == end) {
        lseek(fd, offset+1, SEEK_SET);
        write(fd, argv[2], strlen(argv[2]));
    } else {
        // 이동시킬 만큼의 버퍼 동적 할당
        char* buffer = (char*)malloc(end - offset + 1);
        // 예외 처리
        if (buffer == NULL) {
            fprintf(stderr, "Memory Allocation Error.\n");
            close(fd);
            exit(1);
        }

        // 오프셋 이후의 데이터를 버퍼에 읽어옴
        lseek(fd, offset+1, SEEK_SET);
        read(fd, buffer, end - offset);

        // 오프셋 +1 위치에 데이터 쓰기
        lseek(fd, offset+1, SEEK_SET);
        write(fd, argv[2], strlen(argv[2]));

        // 버퍼의 데이터를 다시 파일에 쓰기
        write(fd, buffer, end - offset);

        free(buffer);
    }
    //잉여 데이터 지워주기
    ftruncate(fd, end+strlen(argv[2]));

    close(fd);
    exit(0);
}



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

    // �������� ������ ���� ������ append ó��
    if (offset == end) {
        lseek(fd, offset+1, SEEK_SET);
        write(fd, argv[2], strlen(argv[2]));
    } else {
        // �̵���ų ��ŭ�� ���� ���� �Ҵ�
        char* buffer = (char*)malloc(end - offset + 1);
        // ���� ó��
        if (buffer == NULL) {
            fprintf(stderr, "Memory Allocation Error.\n");
            close(fd);
            exit(1);
        }

        // ������ ������ �����͸� ���ۿ� �о��
        lseek(fd, offset+1, SEEK_SET);
        read(fd, buffer, end - offset);

        // ������ +1 ��ġ�� ������ ����
        lseek(fd, offset+1, SEEK_SET);
        write(fd, argv[2], strlen(argv[2]));

        // ������ �����͸� �ٽ� ���Ͽ� ����
        write(fd, buffer, end - offset);

        free(buffer);
    }
    //�׿� ������ �����ֱ�
    ftruncate(fd, end+strlen(argv[2]));

    close(fd);
    exit(0);
}



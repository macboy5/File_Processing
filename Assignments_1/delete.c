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

    //����Ʈ �� ������ ���
    if(bytes < 0) {
        //����Ʈ���� ������ offset���� ũ�ų� ���� ��, �� ������ ���� ���� ������ ���� �����Ǵ� ���.
        if(abs(bytes) >= offset) {
            //������ ���� ������ ������ ���� ���ۿ� ����.
            char* buffer = (char*)malloc(end - offset );
            lseek(fd, offset, SEEK_SET);
            read(fd, buffer, end - offset );

            //���� ũ�� 0���� �ʱ�ȭ.
            ftruncate(fd, 0);

            //buffer�� ����� �����͸� �� �տ� write()
            lseek(fd, 0, SEEK_SET);
            write(fd, buffer, end - offset );

            free(buffer);

        } else {
            //����Ʈ���� ������ offset���� ���� ��

            //buffer1�� ������ ������ ������ ����.
            char* buffer1 = (char*)malloc(end - offset );
            lseek(fd, offset, SEEK_SET);
            read(fd, buffer1, end - offset );

            //buffer2�� ������ ���� �� ������ �� �����ǰ� ���� ������ ����.
            char* buffer2 = (char*)malloc(offset - abs(bytes) );
            lseek(fd, 0, SEEK_SET);
            read(fd, buffer2, offset - abs(bytes) );

            //���� ũ�� 0���� �ʱ�ȭ.
            ftruncate(fd, 0);

            //������ �Ǿ����� �ű�� buffer2�� �ִ� ���� �ٿ� �ֱ�.
            lseek(fd, 0, SEEK_SET);
            write(fd, buffer2, offset - abs(bytes) );

            //������ ���� ���� ��ġ�� �ű�� buffer1�� �ִ� ���� �ٿ� �ֱ�.
            lseek(fd, offset - abs(bytes), SEEK_SET);
            write(fd, buffer1, end - offset );

            free(buffer1);
            free(buffer2);

        }
    }
    //����Ʈ �� ���
    else if(bytes > 0) {
        //����Ʈ ���� ������ �������� ������ ������ ũ�ų� ���� ���, �� ������ ������ ���� �� �����ϴ� ���.
        if(bytes >= end - offset) {
            //���ۿ� ������ ���� ���� ������ ����.
            char* buffer = (char*)malloc(offset +1);
            lseek(fd, 0, SEEK_SET);
            read(fd, buffer, offset +1);

            //���� ũ�� 0���� �ʱ�ȭ.
            ftruncate(fd, 0);

            //������ �Ǿ����� �������� ���ۿ� �ִ� ���� �ٿ��ֱ�.
            lseek(fd, 0, SEEK_SET);
            write(fd, buffer, offset +1);

            free(buffer);
        } else {
            //����Ʈ���� ������ ������ ������ �����ٴ� ���� ���

            //���ۿ� �����ϰ� ���� ������ ����.
            char* buffer = (char*)malloc(end - offset - bytes + 1);
            lseek(fd, offset + bytes+1, SEEK_SET);
            read(fd, buffer, end - offset - bytes);

            //offset+1 ��ġ�� ������ �̵� �� �ٿ� �ֱ�.
            lseek(fd, offset+1, SEEK_SET);
            write(fd, buffer, end - offset - bytes);

            //�ڿ� ���� �׿� �����͸� �����ֱ� ���� ����ũ�⸦ end - bytes�� ����.
            ftruncate(fd, end - bytes);

            free(buffer);
        }
    } else {
        exit(1);
    }

    close(fd);
    exit(0);
}

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char* argv[]){
    //����ó��
    if(argc != 4){
		fprintf(stderr, "Input Error : Write <File name 1> <File name 2> <File name 3>.\n");
		exit(1);
	}

    int fd1 = open(argv[1], O_RDONLY);
    int fd2 = open(argv[2], O_RDONLY);

    //����3�� ����1�� ����2�� ������ ���ο� ����
    int fd3 = open(argv[3], O_CREAT|O_WRONLY|O_APPEND,0666);

    if(fd1 < 0 || fd2 < 0 || fd3< 0){
        fprintf(stderr, "Write <File name 1> <File name 2> <File name 3>.\n");
		exit(1);
    }

    char buffer;

    //���� �� �ִ� ���� write()���� �� �� ����.
    while( read(fd1, &buffer, 1) != 0){
        write(fd3, &buffer, 1);
    }
    close(fd1);

    while( read(fd2, &buffer, 1) !=0){
        write(fd3, &buffer, 1);
    }
    close(fd2);

    close(fd3);

    exit(0);
}
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    
    //����ó��
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
    //�������� ���� ũ���� ������ ��� ���
    if(offset > file_size){
        fprintf(stderr, "Out of bounds : Offset is bigger than file size.\n");
        exit(1);
    }

    char* buffer = NULL;
    //����Ʈ �� ����
    if(bytes < 0){
        //bytes�� ������ offset���� �� ũ�ų� ���� �� 0���� offset-1���� �а� ���
        if(abs(bytes) >= offset){
            //������ ��ġ ó������ �̵�.
            lseek(fd, 0, SEEK_SET);

            //offsetũ�⸸ŭ �����Ҵ�
            buffer = (char*)(malloc(offset));
            read(fd, buffer, offset);
            printf("%s", buffer);
        /*    write(fd, buffer, offset);*/
         /*   fwrite(buffer, 1, offset, stdout);*/
        }
        //bytes�� ������ offset���� ���� ���.. offset-bytes���� offset-1���� �а� ���.
        else{
            lseek(fd, offset-abs(bytes), SEEK_SET);
            buffer = (char*)(malloc)((abs(bytes)));
            read(fd, buffer, abs(bytes));
            printf("%s", buffer);
          /*  write(fd, buffer, abs(bytes));*/
          /*  fwrite(buffer, 1, abs(bytes), stdout);*/
        }
    }

    //����Ʈ �� ���
    else if(bytes > 0 ){
        //offset+bytes�� file_size�Ѿ�°��
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

    //����Ʈ �� 0
    else{
        exit(1);
    }

    free(buffer);
    close(fd);
    exit(0);
}
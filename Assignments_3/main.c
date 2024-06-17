//
// 과제3의 채점 프로그램은 기본적으로 아래와 같이 동작함
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "blockmapping.h"
#include <unistd.h>
#include <fcntl.h>


FILE *flashmemoryfp;

/****************  prototypes ****************/
void test_ftl_open_and_print();
void test_basic_write_read();
void test_overwriting_and_verification();
int capture_and_get_pbn_for_lbn1();
void read_pages_upto_pbn(FILE *flash_fp, int pbn, bool all);
void ftl_open();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);
void ftl_print();

//
// 이 함수는 file system의 역할을 수행한다고 생각하면 되고,
// file system이 flash memory로부터 512B씩 데이터를 저장하거나 데이터를 읽어 오기 위해서는
// 각자 구현한 FTL의 ftl_write()와 ftl_read()를 호출하면 됨
//
/****************  test functions ****************/
void test_ftl_open_and_print() {
    printf("### Starting FTL open and print initial mapping table ###\n");
    ftl_open();  // Initialize the FTL system
    ftl_print(); // Print the initial state of the address mapping table and free block
    printf("### End of FTL open and print initial mapping table ###\n\n");
}

void test_basic_write_read() {
    char sectorbuf[SECTOR_SIZE];
    char readbuf[SECTOR_SIZE + 16]; // Assuming the spare area is 16 bytes and includes the LSN right after the sector data
    FILE *flash_fp;
    int lsn_read, page_number, mapped_pbn;
    bool all = true;

    // Test basic write
    char *dataA = "A";
    ftl_write(4, dataA);
    fflush(flashmemoryfp);  // Ensure the buffer is flushed to the file

    printf("### Starting basic write_read ###\n");
    // Test read to verify the write
    memset(sectorbuf, 0, SECTOR_SIZE);
    ftl_read(4, sectorbuf);

    // Print the contents of sectorbuf after reading
    printf("LSN 4: '%s'\n", sectorbuf);

    // Check if the data read is the same as the data written
    if (strcmp(dataA, sectorbuf) == 0) {
        printf("LSN 4 write_read PASSED.\n");
    } else {
        printf("LSN 4 write_read FAILED.\n");
    }
    printf("### End of basic write_read ###\n\n");

    // Additional writes to test sequence writes
    ftl_write(5, "B");
    fflush(flashmemoryfp);  // Ensure the buffer is flushed to the file

    ftl_write(6, "C");
    fflush(flashmemoryfp);  // Ensure the buffer is flushed to the file

    ftl_write(7, "D");
    fflush(flashmemoryfp);  // Ensure the buffer is flushed to the file

    printf("### Starting print mapping table after write ###\n");
    ftl_print(); // Print the table after writes to see the state changes
    printf("### End of print mapping table after write ###\n\n");

    printf("### Starting write IO read_all ###\n");
    // Open the flash memory file for reading directly
    flash_fp = fopen("flashmemory", "rb");
    if (!flash_fp) {
        perror("Failed to open flash memory file\n\n");
        return;
    }

    mapped_pbn = capture_and_get_pbn_for_lbn1();

    if (mapped_pbn != -1) {
        read_pages_upto_pbn(flash_fp, mapped_pbn, all);
    } else {
        printf("Error, No valid PBN found for LBN %d\n", mapped_pbn);
    }

    printf("### End of write IO read_all ###\n\n");

    printf("### Starting write IO read_from_pbn ###\n");
    all = false;
    if (mapped_pbn != -1) {
        read_pages_upto_pbn(flash_fp, mapped_pbn, all);
    } else {
        printf("Error, No valid PBN found for LBN %d\n", mapped_pbn);
    }
    printf("### End of write IO read_from_pbn ###\n\n");

    fflush(flashmemoryfp);  // Ensure the buffer is flushed to the file
    fclose(flash_fp);

}

void test_overwriting_and_verification() {
    printf("### Starting overwrite_read ###\n");
    char sectorbuf[SECTOR_SIZE];
    char readbuf[SECTOR_SIZE + 16];  // Assuming spare area is 16 bytes
    FILE *flash_fp;
    int lsn, page_number, mapped_pbn;
    bool all = true;

    // Performing overwriting tests
    char *dataE = "E";
    ftl_write(4, dataE);
    fflush(flashmemoryfp);  // Ensure the buffer is flushed to the file

    ftl_read(4, sectorbuf);
    printf("LSN 4 overwrite: '%s'\n", sectorbuf);

    // Check if the data read is the same as the data written
    if (strcmp(dataE, sectorbuf) == 0) {
        printf("LSN 4 overwrite_read PASSED.\n");
    } else {
        printf("LSN 4 overwrite_read FAILED.\n");
    }
    printf("### End of overwrite_read ###\n\n");

    char *dataF = "F";
    ftl_write(5, dataF);
    fflush(flashmemoryfp);  // Ensure the buffer is flushed to the file

    char *dataG = "G";
    ftl_write(6, dataG);
    fflush(flashmemoryfp);  // Ensure the buffer is flushed to the file

    char *dataH = "H";
    ftl_write(7, dataH);
    fflush(flashmemoryfp);  // Ensure the buffer is flushed to the file

    printf("### Starting print mapping table after overwrite ###\n");
    ftl_print(); // Print the table to see updates after overwrites
    printf("### End of print mapping table after overwrite ###\n\n");

    printf("### Starting overwrite IO read_all ###\n");
    // Open the flash memory file for reading directly
    flash_fp = fopen("flashmemory", "rb");
    if (!flash_fp) {
        perror("Failed to open flash memory file\n\n");
        return;
    }

    mapped_pbn = capture_and_get_pbn_for_lbn1();

    if (mapped_pbn != -1) {
        read_pages_upto_pbn(flash_fp, mapped_pbn, all);
    } else {
        printf("Error, No valid PBN found for LBN %d\n", mapped_pbn);
    }

    printf("### End of overwrite IO read_all ###\n\n");

    printf("### Starting overwrite IO read_from_pbn ###\n");
    all = false;
    if (mapped_pbn != -1) {
        read_pages_upto_pbn(flash_fp, mapped_pbn, all);
    } else {
        printf("Error, No valid PBN found for LBN %d\n", mapped_pbn);
    }
    printf("### End of overwrite IO read_from_pbn ###\n\n");

    fflush(flashmemoryfp);  // Ensure the buffer is flushed to the file
    fclose(flash_fp);
}


int capture_and_get_pbn_for_lbn1() {
    fflush(stdout); // Ensure the buffer is flushed before redirection
    int original_stdout = dup(STDOUT_FILENO); // Preserve the original stdout

    int fd = open("temp_ftl_output.txt", O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd == -1) {
        perror("Failed to open temporary file");
        return -1;
    }

    dup2(fd, STDOUT_FILENO); // Redirect stdout to the file
    close(fd);

    ftl_print(); // Call ftl_print, output goes to the file

    fflush(stdout); // Make sure all output has been written to the file
    dup2(original_stdout, STDOUT_FILENO); // Restore original stdout
    close(original_stdout);

    FILE *fp = fopen("temp_ftl_output.txt", "r");
    if (!fp) {
        perror("Failed to open temporary file for reading");
        return -1;
    }

    char line[256];
    int lbn, pbn = -1;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%d %d", &lbn, &pbn) == 2) {
            if (lbn == 1) {
                break;
            }
        }
    }

    fflush(flashmemoryfp);  // Ensure the buffer is flushed to the file
    fclose(fp);
    remove("temp_ftl_output.txt"); // Clean up
    return pbn; // Return the found PBN or -1 if not found
}



void read_pages_upto_pbn(FILE *flash_fp, int pbn, bool all) {
    char readbuf[PAGE_SIZE];
    int start_page = pbn * PAGES_PER_BLOCK;
    int end_page = start_page + PAGES_PER_BLOCK;
    int total_pages_up_to_pbn = end_page; // This is the total number of pages up to and including the pbn block
    int lsn_read;

    if (all) {
        // printf("### Reading all pages up to and including PBN %d ###\n", pbn);
        for (int page_number = 0; page_number < total_pages_up_to_pbn; page_number++) {
            fseek(flash_fp, page_number * PAGE_SIZE, SEEK_SET);
            if (fread(readbuf, 1, PAGE_SIZE, flash_fp) == PAGE_SIZE) {
                lsn_read = *((int*)(readbuf + SECTOR_SIZE));
                printf("Page %d - Data: '%.512s' - LSN: %d\n", page_number, readbuf, lsn_read);
            } else {
                printf("Failed to read page %d from flash memory.\n", page_number);
            }
        }
    } else {
        // printf("### Focused read: Pages from PBN %d ###\n", pbn);
        for (int page_number = start_page; page_number < end_page; page_number++) {
            fseek(flash_fp, page_number * PAGE_SIZE, SEEK_SET);
            if (fread(readbuf, 1, PAGE_SIZE, flash_fp) == PAGE_SIZE) {
                lsn_read = *((int*)(readbuf + SECTOR_SIZE));
                // printf("Page %d (PBN %d) - Data: '%.512s' - LSN: %d\n", page_number, pbn, readbuf, lsn_read);
                printf("Page %d - Data: '%.512s' - LSN: %d\n", page_number, readbuf, lsn_read);
            } else {
                printf("Failed to read page %d from PBN %d.\n", page_number, pbn);
            }
        }
    }
}


int main(int argc, char *argv[])
{
    char *blockbuf;
    //char sectorbuf[SECTOR_SIZE];
    int lsn, i;

    // Open flash memory file
    flashmemoryfp = fopen("flashmemory", "w+b");
    if(flashmemoryfp == NULL)
    {
        printf("file open error\n");
        exit(1);
    }

    //
    // flash memory의 모든 바이트를 '0xff'로 초기화한다.
    // Initialize flash memory with all bytes set to '0xFF'
    //
    blockbuf = (char *)malloc(BLOCK_SIZE);
    memset(blockbuf, 0xFF, BLOCK_SIZE);

    for(i = 0; i < BLOCKS_PER_DEVICE; i++)
    {
        fwrite(blockbuf, BLOCK_SIZE, 1, flashmemoryfp);
    }
    free(blockbuf);

    // Run the FTL system open and print test
    test_ftl_open_and_print();

    // Run the basic write and read test
    test_basic_write_read();

    // Run the overwritting test
    test_overwriting_and_verification();

    fflush(flashmemoryfp);  // Ensure the buffer is flushed to the file
    fclose(flashmemoryfp);

    return 0;
}

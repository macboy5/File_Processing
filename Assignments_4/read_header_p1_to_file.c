#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_HEADER_SIZE 16
#define PAGE_SIZE 512
#define PAGE_HEADER_SIZE 64
#define MAX_RECORDS_NUM 28

void read_page_header(FILE *file, unsigned short *num_records, unsigned short *free_space, char *reserved, unsigned short *offsets, int max_records_num) {
    fread(num_records, sizeof(unsigned short), 1, file);
    fread(free_space, sizeof(unsigned short), 1, file);
    fread(reserved, sizeof(char), 4, file);
    fread(offsets, sizeof(unsigned short), max_records_num, file);
}

int main(int argc, char *argv[]) {
    FILE *input_file, *output_file, *output_file_2;
    int total_pages = 3; // Assuming there are 3 pages in the file for this example
    int target_page = 1; // Focus on page 1

    input_file = fopen("students.dat", "rb");
    if (input_file == NULL) {
        perror("Failed to open students.dat");
        return EXIT_FAILURE;
    }

    output_file = fopen("header_output_p1.txt", "w");
    if (output_file == NULL) {
        perror("Failed to open total_pages.txt");
        fclose(input_file);
        return EXIT_FAILURE;
    }
	
    output_file_2 = fopen("total_pages.txt", "w");
    if (output_file_2 == NULL) {
        perror("Failed to open header_output_p1.txt");
        fclose(input_file);
        return EXIT_FAILURE;
    }	

    if (target_page >= total_pages) {
        fprintf(stderr, "Invalid target page number %d. Total pages: %d\n", target_page, total_pages);
        fclose(input_file);
        fclose(output_file);
		fclose(output_file_2);
        return EXIT_FAILURE;
    }

    fseek(input_file, FILE_HEADER_SIZE + target_page * PAGE_SIZE, SEEK_SET);

    unsigned short num_records, free_space;
    char page_reserved[4];
    unsigned short offsets[MAX_RECORDS_NUM]; // Adjust for maximum number of records per page

    read_page_header(input_file, &num_records, &free_space, page_reserved, offsets, MAX_RECORDS_NUM);
	
    //fprintf(output_file, "Page %d:\n", target_page);
    fprintf(output_file, "Number of records: %u\n", num_records);
    fprintf(output_file, "Free space: %u\n", free_space);
    fprintf(output_file, "Offsets: ");
    for (int i = 0; i < num_records; i++) {
        fprintf(output_file, "%u ", offsets[i]);
    }
    fprintf(output_file, "\n");
	
	fprintf(output_file_2, "Total pages: %u\n", total_pages);
	fprintf(output_file_2, "\n");

    fclose(input_file);
    fclose(output_file);
	fclose(output_file_2);
    return EXIT_SUCCESS;
}

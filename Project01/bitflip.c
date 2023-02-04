#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//#define MAX_FILE_SIZE 25000
#define FILE_EXTENSION_bf ".bf"
#define FILE_EXTENSION_bfr ".bfr"
#define FILE_EXTENSION_r ".r"

//function to take in bits and flip them
void flip_bits(char *buffer, int size) {
    for (int i = 0; i < size; i++) {
       buffer[i] = buffer[i] ^ 0xFF;
    }
}
void reverse_buffer(char *buffer, int length) {
  int i, j;
  for (i = 0, j = length - 1; i < length / 2; i++, j--) {
    unsigned char temp = buffer[i];
    buffer[i] = buffer[j];
    buffer[j] = temp;
  }
}

int main(int argc, char *argv[]) {
    int MAX_FILE_SIZE = 25000;
    char *filename = argv[1];
    char *output_file = NULL;
    char *max = NULL;
    int change_output = 0; //flag to check if output file name is changed
    int bffr = 0; //flag to check if bffr file is created

    for (int i = 1; i < argc; i++) {
        //Override output file name
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                output_file = argv[i + 1];
                change_output = 1;
            } 
            else {
                printf("Error: missing output file after -o option\n");
                return 1;
            }
        }
        //override max file size
        else if(strcmp(argv[i], "-maxsize") == 0) {
                if (i + 1 < argc) {
                    max = argv[i + 1];
                    MAX_FILE_SIZE = atoi(max);
                }
                else {
                    printf("Error: missing max file size after -maxsize option\n");
                    return 1;
                }
        }
        else if(strcmp(argv[i], "-r") == 0) {
            bffr = 1;
        }
        else if(strcmp(argv[i], "-bfr") == 0) {
            bffr = 2;
        }
    }

    //write name of output file
    char *bf_filename = malloc(strlen(filename) + strlen(FILE_EXTENSION_bf) + 1);
    if (change_output == 1) {
        strcpy(bf_filename, output_file);
    }
    else if(bffr == 1) {
        strcpy(bf_filename, filename);
        strcat(bf_filename, FILE_EXTENSION_r);
    }
    else if(bffr == 2) {
        strcpy(bf_filename, filename);
        strcat(bf_filename, FILE_EXTENSION_bfr);
    }
    else {
        strcpy(bf_filename, filename);
        strcat(bf_filename, FILE_EXTENSION_bf);
    }
    //strcpy(bf_filename, filename);
    //strcat(bf_filename, FILE_EXTENSION_bf);

    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: The file %s does not exist or is not accessible\n", filename);   //file does not exist
        return 1;
    }

    //check if file is within 25 kb
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    if (size > MAX_FILE_SIZE) {
        printf("Error: Input file %s was too large (more than %dkb)!\n", filename, MAX_FILE_SIZE);
        return 1;
    }

    //check if bf file already exists
    if(access(bf_filename, F_OK) != -1) {
        printf("Error: %s already exists!\n", bf_filename);
        return 1;
    }

    //set pointer back to beginning of file
    fseek(file, 0, SEEK_SET);

    //read file into buffer
    char *buffer = malloc(size);
    if (fread(buffer, 1, size, file) != size) {
        printf("Error reading %s\n", filename);
        return 1;
    }
    fclose(file);

    if(bffr == 1) {
        reverse_buffer(buffer, size);   // -bfr option
    }
    else if( bffr == 2){
        reverse_buffer(buffer, size);   // -r option
        flip_bits(buffer, size);
    }
    else{
        flip_bits(buffer, size);    //default bit flip
    }

    //write flipped buffer to new file
    file = fopen(bf_filename, "wb");
    if (fwrite(buffer, 1, size, file) != size) {
        printf("Error writing %s", bf_filename);
        return 1;
    }
    fclose(file);

    //output message
    printf("Input: %s was %d bytes\n", filename, size);
    printf("Output: %s was output successfully\n", bf_filename);

    return 0;
}

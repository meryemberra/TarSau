#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILES 32
#define MAX_TOTAL_SIZE 200 * 1024 * 1024 // 200 MB

void printUsage() {
    printf("Usage: tarsau -b -o <output_file> <input_file1> <input_file2> ... <input_fileN>\n");
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printUsage();
        return 1;
    }

    int binaryFlag = 0; 
    char *outputFileName = "a.sau"; // Default output file name

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-b") == 0) {
            binaryFlag = 1;
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                outputFileName = argv[i + 1];
                i++; // Skip the next argument as it is the output file name
            } else {
                printf("Error: Missing output file name after -o\n");
                printUsage();
                return 1;
            }
        }
    }

    FILE *outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL) {
        perror("Error creating output file");
        return 1;
    }

    size_t totalSize = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-b") != 0 && strcmp(argv[i], "-o") != 0) {
            FILE *inputFile = fopen(argv[i], "r");
            if (inputFile == NULL) {
                perror("Error opening input file");
                fclose(outputFile);
                return 1;
            }
            int isTextFile = 1;

            if (!isTextFile) {
                printf("%s input file format is incompatible!\n", argv[i]);
                fclose(inputFile);
                fclose(outputFile);
                return 1;
            }

            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) > 0) {
                fwrite(buffer, 1, bytesRead, outputFile);
                totalSize += bytesRead;

                if (totalSize > MAX_TOTAL_SIZE) {
                    printf("Error: Total size exceeds 200 MB\n");
                    fclose(inputFile);
                    fclose(outputFile);
                    return 1;
                }
            }

            fclose(inputFile);
        }
    }

    fclose(outputFile);

    return 0;
}

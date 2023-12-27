#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_FILES 32
#define MAX_FILE_SIZE 200 * 1024 * 1024 // 200 MB

// Function prototypes
void mergeFiles(const char *outputFileName, const char *fileNames[], int fileCount);
void extractFiles(const char *archiveFileName, const char *outputDirectory);

// File information structure
struct FileInfo {
    char fileName[256];
    char permissions[9];
    size_t size;
};

// Function to check if a file is a text file
int isTextFile(const char *fileName) {
 
    return 1; 
}

// Function to merge text files into an archive
void mergeFiles(const char *outputFileName, const char *fileNames[], int fileCount) {
    FILE *outputFile = fopen(outputFileName, "wb");
    if (outputFile == NULL) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    // Write the size of the organization section (first 10 bytes)
    size_t orgSectionSize = 0;
    fwrite(&orgSectionSize, sizeof(size_t), 1, outputFile);

    // Write the organization section records
    for (int i = 0; i < fileCount; ++i) {
        if (!isTextFile(fileNames[i])) {
            fprintf(stderr, "Error: %s is not a text file. Skipping...\n", fileNames[i]);
            continue;
        }

        FILE *inputFile = fopen(fileNames[i], "rb");
        if (inputFile == NULL) {
            perror("Error opening input file");
            fclose(outputFile);
            exit(EXIT_FAILURE);
        }

        // Get file permissions
        struct stat fileStat;
        stat(fileNames[i], &fileStat);
        sprintf(orgSectionRecord.permissions, "%o", fileStat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));

        // Get file size
        fseek(inputFile, 0, SEEK_END);
        orgSectionRecord.size = ftell(inputFile);
        fseek(inputFile, 0, SEEK_SET);

        // Write the organization section record
        fwrite(&orgSectionRecord, sizeof(struct FileInfo), 1, outputFile);
        orgSectionSize += sizeof(struct FileInfo);

        // Write the file contents
        char buffer[MAX_FILE_SIZE];
        size_t bytesRead;
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) > 0) {
            fwrite(buffer, 1, bytesRead, outputFile);
        }

        fclose(inputFile);
    }

    // Update the size of the organization section in the file
    fseek(outputFile, 0, SEEK_SET);
    fwrite(&orgSectionSize, sizeof(size_t), 1, outputFile);

    fclose(outputFile);
}

// Function to extract files from an archive
void extractFiles(const char *archiveFileName, const char *outputDirectory) {
    // TODO: Implement this function to extract files from the archive
}

int main(int argc, char *argv[]) {
    if (argc < 4 || (strcmp(argv[1], "-b") != 0 && strcmp(argv[1], "-a") != 0)) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s -b file1 file2 ... -o output.sau\n", argv[0]);
        fprintf(stderr, "  %s -a input.sau output_directory\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-b") == 0) {
        // Archive operation
        const char *outputFileName = (argc > 3) ? argv[argc - 1] : "a.sau";
        mergeFiles(outputFileName, argv + 2, argc - 4);
        printf("The files have been merged.\n");
    } else if (strcmp(argv[1], "-a") == 0) {
        // Extract operation
        const char *archiveFileName = argv[2];
        const char *outputDirectory = argv[3];
        extractFiles(archiveFileName, outputDirectory);
    }

    return 0;
}


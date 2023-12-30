#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#define MAX_FILES 32
#define MAX_TOTAL_SIZE 200 * 1024 * 1024 // 200 MB



typedef struct
{
    char fileName[100];
    char *content;
    off_t size;
    mode_t permissions;
} FileInfo;


char FullContent[100][200][200]; //array for content storage

int findContents(FileInfo *fileInfos, char *archiveFileName, int fileCount) {
    FILE *file = fopen(archiveFileName, "r");
    if (file == NULL) {
        printf("Unable to open the file.\n");
        return 1;
    }

    char line[200]; // Temporary storage for each line
    int lineCount = 0;

    // Skip lines until (fileCount + 1) line
    for (int i = 0; i < (fileCount + 1); i++) {
        if (fgets(line, sizeof(line), file) == NULL) {
            printf("Insufficient content available.\n");
            fclose(file);
            return 1;
        }
    }

    int currentFileIndex = 0;
    int lineNumber = 0;

    while (currentFileIndex < fileCount) {
        int lineTotalSize = 0;

        while (lineTotalSize < fileInfos[currentFileIndex].size) {
            if (fgets(line, sizeof(line), file) != NULL) {
                snprintf(FullContent[currentFileIndex][lineNumber], sizeof(line), "%s", line);
                lineTotalSize += strlen(line);
                lineNumber++;
            } else {
                break; //end of the file
            }
        }

        // Assign content to FileInfo's content
        fileInfos[currentFileIndex].content = (char *)malloc(lineTotalSize + 1);
        strcpy(fileInfos[currentFileIndex].content, "");

        for (int i = 0; i < lineNumber; i++) {
            strcat(fileInfos[currentFileIndex].content, FullContent[currentFileIndex][i]);
        }

        currentFileIndex++;
        lineNumber = 0;
    }

    fclose(file);
    return 0;
}


void extractArchive(char *archiveFileName, char *outputFolderName) {
    
    if (mkdir(outputFolderName, 0777) == -1) {
        perror("mkdir");
        exit(EXIT_FAILURE);
    }
    
    FILE *archiveFile = fopen(archiveFileName, "r");
    if (archiveFile == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    int lineCount = 0;
    int fileCount = 0;
    char line[1000];
    FileInfo fileInfos[MAX_FILES];
    
    int startingLine = 2;
    while (lineCount < startingLine - 1) {
        fgets(line, sizeof(line), archiveFile);
        lineCount++;
    }

    while (fgets(line, sizeof(line), archiveFile) != NULL && fileCount < MAX_FILES) {
       if (line[0] == '|') {
        char fileName[100];
        int permissions;
        long size;

          if (sscanf(line, "|%99[^,],%o,%ld|", fileName, &permissions, &size) == 3) {
            snprintf(fileInfos[fileCount].fileName, sizeof(fileInfos[fileCount].fileName), "%s", fileName);
            fileInfos[fileCount].permissions = permissions;
            fileInfos[fileCount].size = size;
            fileCount++;
          } else {

            fprintf(stderr, "An error occurred while reading file information. Line: %s", line);
           }
        }
    }
 

    printf("%d : bu file count \n\n", fileCount);
    for (int i = 0; i < fileCount; i++) {
        printf("File Name: %s\n", fileInfos[i].fileName);
        printf("Permissions: %o\n", fileInfos[i].permissions);
        printf("Size: %ld\n", fileInfos[i].size);
        printf("------------\n");
    }

    fclose(archiveFile);
    
   int findContentsResult= findContents(fileInfos,archiveFileName, fileCount);
    
    if (findContentsResult == 0) {
    for (int i = 0; i < fileCount; i++) {
        char filePath[100];
        snprintf(filePath, sizeof(filePath), "%s/%s", outputFolderName, fileInfos[i].fileName);

        FILE *file = fopen(filePath, "w");
        if (file == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        fprintf(file, "%s", fileInfos[i].content);

        fclose(file);
    }
  }

}

   


void writeToArchive(FileInfo *fileInfos, const char *outputFileName, int fileNameCount, off_t totalSize) {
    FILE *archiveFile = fopen(outputFileName, "w");
    if (archiveFile == NULL) {
        printf("An error occurred while creating the archive file!\n");
        return;
    }

    fprintf(archiveFile, "%010ld", totalSize);
    fprintf(archiveFile,"\n");

    // Write the file information
    for (int i = 0; i < fileNameCount; i++) {
        fprintf(archiveFile, "|%s,%o,%ld|\n", fileInfos[i].fileName, fileInfos[i].permissions, fileInfos[i].size);
    }

    for (int i = 0; i < fileNameCount; i++) {
        int fd = open(fileInfos[i].fileName, O_RDONLY);
        if (fd < 0) {
            printf("An error occurred while opening the file!\n");
            continue;
        }

        char buffer[1024];
        ssize_t bytesRead;

        while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
            fwrite(buffer, sizeof(char), bytesRead, archiveFile);
        }
        close(fd);
    }

    fclose(archiveFile);
    printf("The files have been merged.\n");
}




void getFileInfos(char *fileNameArray[], int fileNameCount, const char *outputFileName) {
    FileInfo fileInfos[MAX_FILES];
    off_t totalSize = 0;

    for (int i = 0; i < fileNameCount; ++i) {
        struct stat fileStat;
        if (stat(fileNameArray[i], &fileStat) == -1) {
            perror("An error occurred while getting the file size!");
            continue;
        }

        if (totalSize + fileStat.st_size <= MAX_TOTAL_SIZE) {
            totalSize += fileStat.st_size;

            strncpy(fileInfos[i].fileName, fileNameArray[i], sizeof(fileInfos[i].fileName) - 1);
            fileInfos[i].fileName[sizeof(fileInfos[i].fileName) - 1] = '\0';

            fileInfos[i].permissions = fileStat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
            fileInfos[i].size = fileStat.st_size;

            int fd = open(fileNameArray[i], O_RDONLY);
            if (fd < 0) {
                perror("Couldn't open the file");
                continue;
            }

            fileInfos[i].content = (char *)malloc((fileStat.st_size + 1) * sizeof(char));
            if (fileInfos[i].content == NULL) {
                perror("An error occurred while allocating memory");
                close(fd);
                continue;
            }

            ssize_t bytesRead = read(fd, fileInfos[i].content, fileStat.st_size);
            if (bytesRead != fileStat.st_size) {
                perror("An error occurred while reading the file");
                close(fd);
                free(fileInfos[i].content);
                continue;
            }

            fileInfos[i].content[fileStat.st_size] = '\0';

            close(fd);
        } else {
            printf("%s could not be added because totalSize exceeded 200MB\n", fileNameArray[i]);
        }
    }
    for (int i = 0; i < fileNameCount; ++i) {
        printf("%s",fileInfos[i].content);
    }

    writeToArchive(fileInfos, outputFileName, fileNameCount, totalSize);
    for (int i = 0; i < fileNameCount; ++i) {
        free(fileInfos[i].content);
    }
}




int isTextFile(const char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file) {
        int c;
        while ((c = fgetc(file)) != EOF) {
            if (c < 0 || c > 127) {
                fclose(file);
                return 0; // ASCII aralığında olmayan bir karakter bulundu, metin dosyası değil
            }
        }
        fclose(file);
        return 1; 
    }
    return 0; //The file couldnt open or read
}



int chooseParameter(int argc, char *argv[]){

     if (argc < 4 || (strcmp(argv[1], "-b") == 0 && strcmp(argv[1], "-a") == 0))
    {
        printf("Usage: %s -b input_files -o output_file\n", argv[0]);
        printf("       %s -a archive_file directory\n", argv[0]);
        return EXIT_FAILURE;
    }
    // for -b operations
    else if (strcmp(argv[1], "-b") == 0)
    {
        char *outputFileName = (strcmp(argv[argc - 2], "-o") == 0) ? argv[argc - 1] : "a.sau";



        char *fileNameArray[MAX_FILES];
        int fileNameCount = 0;

        for (int i = 2; i < argc; i++)
        {
            if (strcmp(argv[i], "-o") == 0)
            {
                break;
            }
            else
            {
                if (isTextFile(argv[i]))
                {
                    if (fileNameCount < MAX_FILES)
                    {
                        fileNameArray[fileNameCount++] = argv[i];
                    }
                    else
                    {
                        printf("You cannot merge more than 32 text files\n");
                        return EXIT_FAILURE;
                    }
                }
                else if (isTextFile(argv[i]))
                {
                    printf("%s file isn't a ASCII text file.",argv[i]);
                }
            }
        }

        if (fileNameCount == 0)
        {
            printf("There is no text file among your arguments\n");
            return EXIT_FAILURE;
        }

        getFileInfos(fileNameArray, fileNameCount, outputFileName);
    }
    
    // for -a operation
    else if (strcmp(argv[1], "-a") == 0)
    {
        if (argc < 3)
        {
            printf("Usage: %s -a archive_file optional[extract_directory]\n", argv[0]);
            return EXIT_FAILURE;
        }
        if (strstr(argv[2], ".sau") != NULL)
        {
            char *archiveFileName = argv[2];
            char *outputFolderName = argc == 4 ? argv[3] : "d1";
            extractArchive(archiveFileName, outputFolderName);
        }
        else
        {
            printf("Archive file is inappropriate or corrupt!\n");
        }
    }else {
        printf("Invalid arguments\n");
        return EXIT_FAILURE;
    }
return EXIT_SUCCESS;
}


int main(int argc, char *argv[])
{
   chooseParameter(argc,argv);
    return EXIT_SUCCESS;
}apooseParameter 

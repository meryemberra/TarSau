#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_FILES 32
#define MAX_SIZE 200 * 1024 * 1024 //can be up to 200 mb

typedef struct{

size_t size;
char file_name[256];
char permissions[10];

}FileContent;

int main(int argc, char *argv[]){


return 0;
}

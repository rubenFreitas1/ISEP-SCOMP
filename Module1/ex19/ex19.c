#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define MAX_FILENAME 256

int main(){
    char filename[MAX_FILENAME];
    char destination[MAX_FILENAME + 20];

    printf("fileName: ");
    scanf("%s", filename);

    pid_t pid = fork();
    if (pid < 0) {
        perror("Error fork");
        return 1;
    }

    if (pid == 0) {
        execlp("cp", "cp", filename, "backup/", NULL);
        perror("Error with cp");
        exit(1);
    } else {
        wait(NULL);
        printf("COPY DONE.\n");
    }
}
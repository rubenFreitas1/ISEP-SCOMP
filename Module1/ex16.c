#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

int main(){
    char line[MAX_LINE];
    char *args[MAX_ARGS];
    while(1){
        printf("rubenfreitas@LAPTOP-6RMOABBP:/mnt/c/vs_projects/scomp-exercises/Module1$ ");
        if (!fgets(line, MAX_LINE, stdin)) break;
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "end") == 0) break;

        int i = 0;
        char *token = strtok(line, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        pid_t p = fork();
        if(p == 0){
            execvp(args[0], args);
            perror("error");
            exit(EXIT_FAILURE);
        }
        wait(NULL);
    }

}
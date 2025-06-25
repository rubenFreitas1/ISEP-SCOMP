#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

int main(int argc, char *argv[]){
    if(argc < 2){
        perror("error");
        exit(EXIT_FAILURE);
        return 0;
    }
    pid_t pids[argc-1];
    for(int i = 0; i < argc -1; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if(pids[i] == 0){
            char *args[] = { argv[i+1], NULL };
            execvp(argv[i+1], args);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 1; i < argc -1; i++) {
            wait(NULL);
    }
    return 0;
}

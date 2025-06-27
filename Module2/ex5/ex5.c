#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define TIME_SLICE 5

int main(int argc, char* argv[]){
    char path[256];
    if(argc < 2){
        fprintf(stderr, "Usage: %s prog1 prog2 ... progn\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int numProcesses = argc -1;
    pid_t pids[numProcesses];
    int finished[numProcesses];
    memset(finished, 0, sizeof(finished));

    for(int i = 0; i < numProcesses; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            perror("error fork");
            exit(EXIT_FAILURE);
        }

        if(pids[i] == 0){
            kill(getpid(), SIGSTOP);
            sprintf(path, "./build/%s", argv[i+1]);
            execlp(path, argv[i+1], NULL);
            perror("exec failed \n");
            exit(EXIT_FAILURE);
        }
    }

    int remaining = numProcesses;
    while(remaining > 0){
        for (int i = 0; i < numProcesses; i++) {
            if (finished[i]){
                continue;
            }
            kill(pids[i], SIGCONT);
            sleep(TIME_SLICE);
            int status;
            pid_t result = waitpid(pids[i], &status, WNOHANG);
            if (result == pids[i]) {
                finished[i] = 1;
                remaining--;
                printf("[PARENT] Process %d finished\n", pids[i]);
            }else{
                kill(pids[i], SIGSTOP);
            }
        }
    }

    return 0;
}
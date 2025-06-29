#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define CHILDS 5

int main(int argc, char *argv[]){
    if(argc < 2){
        perror("Not enough arguments!");
        exit(EXIT_FAILURE);
    }

    FILE* file;
    sem_t *sem;
    pid_t pids[CHILDS];
    
    if( (sem = sem_open("/sem", O_CREAT, 0644, 1)) == SEM_FAILED){
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < CHILDS; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if(pids[i] == 0){
            sem_wait(sem);
            file = fopen(argv[1], "a");
            if(file == NULL){
                perror("fopen");
                sem_post(sem);
                exit(EXIT_FAILURE);
            }
            fprintf(file, "I am the process with PID %d\n", getpid());
            sleep(2);
            fclose(file);
            sem_post(sem);
            exit(1);
        }
    }

    for(int i = 0; i < CHILDS; i++){
        wait(NULL);
    }

    sem_close(sem);
    sem_unlink("/sem");

    return 0;
}
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

int main(){
    sem_t *sem_write;
    FILE *file;
    pid_t pids[CHILDS];
    if((sem_write = sem_open("/sem_write",O_CREAT,0644,1))==SEM_FAILED){
        perror("sem_open sem_read");
        exit(4);
    }


    for(int i = 0; i < CHILDS; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if(pids[i] == 0){
            sem_wait(sem_write);

            file = fopen("numbers.txt", "a");
            if(file == NULL){
                perror("fopen");
                sem_post(sem_write); // Libertar semáforo se falhar
                exit(1);
            }
            for(int j = 0; j < 200; j++){
            fprintf(file, "%d ", j); 
            }
            fprintf(file, "\n"); 
            fclose(file);
            sem_post(sem_write);
            exit(1);
        }
        
    }

    for(int i = 0; i < CHILDS; i++){
        wait(NULL);
    }

    file = fopen("numbers.txt", "r");
    if(file == NULL){
        perror("fopen read");
        exit(1);
    }

    char ch;
    while((ch = fgetc(file)) != EOF){
        putchar(ch);
    }
    fclose(file);

    
    if (sem_write != SEM_FAILED) {
        sem_close(sem_write);
        sem_unlink("/sem_write");
    }

    return 0;
}
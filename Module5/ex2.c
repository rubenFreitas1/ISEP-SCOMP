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
    sem_t *sem[CHILDS];
    char sem_name[CHILDS][20];
    FILE *file;
    pid_t pids[CHILDS];
    
    for(int i = 0; i < CHILDS; i++) {
        sprintf(sem_name[i], "/sem_child_%d", i);
        if(i == 0){
            sem[i] = sem_open(sem_name[i], O_CREAT, 0644, 1); // Primeiro liberto
        } else {
            sem[i] = sem_open(sem_name[i], O_CREAT, 0644, 0); // Restantes bloqueados
        }

        if(sem[i] == SEM_FAILED){
            perror("sem_open");
            exit(1);
        }
    }


    for(int i = 0; i < CHILDS; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if(pids[i] == 0){
            sem_wait(sem[i]);

            file = fopen("numbers.txt", "a");
            if(file == NULL){
                perror("fopen");
                exit(1);
            }
            int start = i*20 + 1;
            int end = start + 20;
            for(int j = start; j < end; j++){
            fprintf(file, "%d ", j); 
            }
            fprintf(file, "\n"); 
            fclose(file);
            if(i+1 < CHILDS){
                sem_post(sem[i+1]);
            }
             for(int k = 0; k < CHILDS; k++){
                sem_close(sem[k]);
            }
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

    for(int i = 0; i < CHILDS; i++){
        sem_unlink(sem_name[i]);
    }
    return 0;
}
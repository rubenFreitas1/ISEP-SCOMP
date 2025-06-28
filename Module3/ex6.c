#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define PROCESSES 5
#define NUMBER_MAX 100
#define NUMBER_PER_PROCESS 20

int main(){
    int parcial;
    pid_t pids[PROCESSES];
    int fd[PROCESSES][2];
    int vet1[NUMBER_MAX];
    int vet2[NUMBER_MAX];
    int result[NUMBER_MAX];
    memset(result, 0 , sizeof(result));
    for(int j = 0; j < NUMBER_MAX; j++){
        vet1[j] = 1;
        vet2[j] = 1; 
    }
    for(int w = 0; w< PROCESSES; w++){
        if(pipe(fd[w]) == -1){
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < PROCESSES; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if(pids[i] == 0){
            close(fd[i][0]);
            for(int w = 0; w < PROCESSES; w++){
                if(w != i){
                    close(fd[w][0]);
                    close(fd[w][1]);
                }
            }
            int start = i*NUMBER_PER_PROCESS;
            int end = start + NUMBER_PER_PROCESS;
            for(int z = start; z < end; z++){
                parcial = vet1[z] + vet2[z];
                write(fd[i][1], &parcial, sizeof(int));
            }
            close(fd[i][1]);
            exit(EXIT_SUCCESS);
        }
    }
    int sum;
    for(int z = 0; z < PROCESSES; z++){
        waitpid(pids[z], NULL,0);
        close(fd[z][1]);
        int start = z * NUMBER_PER_PROCESS;
        int end  = start + NUMBER_PER_PROCESS;
        for(int b = start; b < end; b++){
            read(fd[z][0], &sum, sizeof(int));
            result[b] = sum; 
        }
        close(fd[z][0]);
    }
    
    for (int a = 0; a < NUMBER_MAX; a++) {
        if (result[a] != 2) {
            printf("Erro no índice %d: valor = %d\n", a, result[a]);
        }
    }

    return 0;
}
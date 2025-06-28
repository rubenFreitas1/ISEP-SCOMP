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
#define MAX_NUMBER 100
#define NUMBER_PER_PROCESS 20


int main(){
    pid_t pids[PROCESSES];
    int fd[2];
    int array1[MAX_NUMBER];
    int array2[MAX_NUMBER];
    int result=0, parcial_result=0;
    for(int i=0; i<MAX_NUMBER; i++){
        array1[i] = 1; 
        array2[i] = 1; 
    }

    if(pipe(fd) == -1){
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < PROCESSES; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if(pids[i] == 0){
            close(fd[0]);
            int start = i*NUMBER_PER_PROCESS;
            int end = start + NUMBER_PER_PROCESS;
            int partialSum = 0;
            for(int j = start; j < end; j++){
                partialSum+= array1[j] + array2[j];
            }
            write(fd[1], &partialSum, sizeof(int));
            close(fd[1]);
            exit(EXIT_SUCCESS);
        }
    }
    close(fd[1]);
    for(int i = 0; i < PROCESSES; i++){
        read(fd[0], &parcial_result, sizeof(int));
        result += parcial_result;
    }    
    printf("Result: %d \n", result);
    close(fd[0]);
    for (int j=0; j<PROCESSES; j++) {
	    waitpid(pids[j], NULL, 0);                             
	} 
    return 0;

}
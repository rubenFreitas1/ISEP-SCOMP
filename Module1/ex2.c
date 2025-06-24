#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

#define NUMBER_PROCESSES 2

int main(){
    pid_t pids[NUMBER_PROCESSES];

    for(int i = 0; i < NUMBER_PROCESSES; i++ ){
        pids[i] = fork();
        if(pids[i] == -1){
        perror("failed fork:");
        exit(1);
        }
        if(pids[i] == 0){
            if(i == 0){
                printf("I am the first child! // PID: %d \n", getpid()); 
                sleep(5);
                exit(1);
            }else if(i == 1){
                printf("I am the second child! // PID: %d \n", getpid()); 
                exit(2);
            }
        }
    }

    printf("I am the father! // PID: %d \n", getpid());
    
    for(int j = 0; j < NUMBER_PROCESSES; j++){
        if(pids[j] > 0){
			wait(NULL);
		}
    }

    return 0;
}
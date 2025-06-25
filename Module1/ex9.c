#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

#define PROCESS 6
#define NUMBER 10

int main(){
    pid_t pids[PROCESS];


    for(int i = 0; i < PROCESS; i++){
        pids[i]= fork();
        if(pids[i] == -1){
            perror("error:");
            exit(1);
        }
        if(pids[i] == 0){
            int start = NUMBER*i + 1;
            int end = start + NUMBER;
            for(int j = start; j < end; j++){
                printf("%d\n", j);
            }
            exit(0);
        }
        wait(NULL);
    }
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>


#define PROCESSES 10
#define ROUNDS 10

typedef struct{
    char message[200];
    int round;
}Object;

int main(){
    pid_t pids[PROCESSES];
    int fd[2];
    int round;
    int roundWinners[ROUNDS];
    int status;
    memset(roundWinners, 0, sizeof(roundWinners));
    if(pipe(fd) == -1){
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }


    for(int i = 0; i< PROCESSES; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        Object object;
        if(pids[i] == 0){
            close(fd[1]);
            read(fd[0], &object, sizeof(Object));
            printf("[CHILD WINNER] PID: %d || MESSAGE: %s \n", getpid(), object.message);
            close(fd[0]);
            exit(object.round);
        }
    }
    Object newObject;
    strcpy(newObject.message, "Win!");
    for(int w = 1; w < ROUNDS + 1; w++){
        newObject.round = w;
        write(fd[1], &newObject, sizeof(Object));
        sleep(1);
    }
    close(fd[1]);
    for(int i = 0; i < PROCESSES; i++){
        waitpid(pids[i], &status, 0);
        if(WIFEXITED(status)){
            int code = WEXITSTATUS(status);
            printf("[CHILD %d] ROUND WIN: %d \n", i+1, code);
        }
    }
    return 0;
}
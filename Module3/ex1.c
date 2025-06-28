#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

int main(){
    pid_t pid;
    int readP;
    int fd[2];
    if(pid == -1){
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if(pipe(fd) == -1){
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if(pid == 0){
        close(fd[1]);
        read(fd[0], &readP, sizeof(int));
        printf("[CHILD] PARENT PID: %d\n", readP);
        exit(1);
    }
    close(fd[0]);
    printf("[PARENT] PID: %d\n", getpid());
    int writeP = getpid();
    write(fd[1], &writeP, sizeof(int));
    wait(NULL);
    return 0;
}
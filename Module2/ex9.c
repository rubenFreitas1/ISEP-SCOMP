#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

void handle_signal(int sig){
    printf("Handled SIGUSR1\n");
}

int main(){
    pid_t pid;

    pid = fork();
    if(pid == -1){
        perror("fork failed:");
        exit(EXIT_FAILURE);
    }
    if(pid == 0){

        struct sigaction act;
        memset(&act, 0, sizeof(struct sigaction));
        act.sa_handler = handle_signal;
        act.sa_flags = SA_RESTART;
        sigaddset(&act.sa_mask, SIGUSR2);
        sigaction(SIGUSR1, &act, NULL);
        while (1) {
            pause();
        }
    }else{
        while (1) {
            sleep(2);
            kill(pid, SIGUSR1);
        }
    }
    return 0;
}
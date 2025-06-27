#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

//a) Só da print uma vez.

void handle_signal(int sig){
    execlp("./build/scomp", "./build/scomp", NULL);
}

int main(){
    pid_t pid;
    int i;
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = handle_signal;
    act.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &act, NULL);

    pid = fork();
    if(pid == 0){
         for(i = 0; i < 5 ; i++){
            kill(getppid(), SIGUSR1);
            sleep(2);
        }
    }else{
        for(;;){
            pause();
        }
    }
   
}
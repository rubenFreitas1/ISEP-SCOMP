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
    execlp("./build/scomp", "./build/scomp", NULL);
}

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("Not enough arguments\n");
        return 0;
    }
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = handle_signal;
    act.sa_flags = 0;
    sigaction(SIGFPE, &act, 0);

    int numb1 = atoi(argv[1]);
    int numb2 = atoi(argv[2]);
    int result =  numb1 / numb2;
    printf("Result: %d \n", result);
    return 0;
}
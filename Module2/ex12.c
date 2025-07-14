#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>


void handler(int sig, siginfo_t *info, void *context){
    char msg[80];
    snprintf(msg, 80, "Captured a SIGUSR1 sent by the process with PID %d\n", info->si_pid);
    write(STDOUT_FILENO, msg, strlen(msg));
}


int main(){
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        struct sigaction sa;
        sa.sa_sigaction = handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_SIGINFO;
        sigaction(SIGUSR1, &sa, NULL);
        printf("Child process (PID %d) waiting for SIGUSR1...\n", getpid());
        while (1) {
            pause();
        }
        exit(0);
    } 
    sleep(1); 
    printf("Parent (PID %d) sending SIGUSR1 to child (PID %d)\n", getpid(), pid);
    kill(pid, SIGUSR1);
    sleep(1); 
    kill(pid, SIGKILL);
    wait(NULL);
    printf("Parent finished.\n");
    return 0;
}
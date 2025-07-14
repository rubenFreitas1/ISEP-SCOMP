#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define CHILDS 50
int success_count = 0;
int finished_count = 0;

void handler_sigusr1(int sig) {
    success_count++;
    finished_count++;
}

void handler_sigusr2(int sig) {
    finished_count++;
}

int simulate1(){
    sleep(rand() % 3 + 1);  
    return rand() % 2; 
}

void simulate2(){
    printf("Process %d: Executing simulate2()\n", getpid());
    sleep(rand() % 3 + 1);
}

volatile sig_atomic_t start_sim2 = 0;

void child_handler(int sig) {
    start_sim2 = 1;
}

int main(){
    pid_t pids[CHILDS];
    srand(time(NULL));

    struct sigaction sa1, sa2;
    sa1.sa_handler = handler_sigusr1;
    sigemptyset(&sa1.sa_mask);
    sa1.sa_flags = 0;
    sigaction(SIGUSR1, &sa1, NULL);

    sa2.sa_handler = handler_sigusr2;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;
    sigaction(SIGUSR2, &sa2, NULL);


    for(int i = 0; i < CHILDS; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            perror("fork failed:");
            exit(EXIT_FAILURE);
        }
        if(pids[i] == 0){
            struct sigaction sa_child;
            sa_child.sa_handler = child_handler;
            sigemptyset(&sa_child.sa_mask);
            sa_child.sa_flags = 0;
            sigaction(SIGUSR1, &sa_child, NULL);

            int result = simulate1();

            kill(getppid(), result ? SIGUSR1 : SIGUSR2);

            while (!start_sim2) {
                pause();
            }

            simulate2();
            exit(0);
        }
    }

    while (finished_count < 25) {
        pause();
    }

    if(success_count == 0){
        printf("Inefficient Algorithm!\n");
        for (int i = 0; i < CHILDS; i++) {
            kill(pids[i], SIGKILL);
        }
    }else{
        for (int i = 0; i < CHILDS; i++) {
            kill(pids[i], SIGUSR1);
        }
    }

    for (int i = 0; i < CHILDS; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return 0;
}
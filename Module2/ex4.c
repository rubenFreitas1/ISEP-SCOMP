#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define MAX_NUMBER 100
#define CHECK_NUMBERS 20
#define PROCESSES 5


void fill_array(int* array){
    int used[100] = {0};
    srand(time(NULL));

    for (int i = 0; i < 100; i++) {
        int num;
        do {
            num = rand() % (100 * 2); 
        } while (used[num] != 0);
        array[i] = num;
        used[num] = 1;
    }
}

void handle_signal(int sig){
	 const char *msg;
	if(sig == SIGUSR1){
		msg = "I got signal SIGUSR1\n";	
	}
	write(STDOUT_FILENO, msg, strlen(msg));
}

int main(){ 
    int numbers[MAX_NUMBER];
    fill_array(numbers);
    pid_t pids[PROCESSES];
    int exp = 10;
    int found = 0;
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = handle_signal;
    act.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &act, NULL);
    int status;
    for(int i = 0; i < PROCESSES; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if(pids[i] == 0){
            int start = i*CHECK_NUMBERS;
            int end = start + CHECK_NUMBERS;
            for(int j = start; j < end; j++){
                if(exp == numbers[j]){
                    printf("[CHILD %d] FOUND NUMBER %d || POSITION %d \n", i+1 , numbers[j], j+1);
                    kill(getppid(), SIGUSR1);
                    exit(i+1);
                }
            }
            exit(0);
        }
    }

    for(int i = 0; i < PROCESSES; i++){
        wait(&status);
        if(WIFEXITED(status)){
            int code = WEXITSTATUS(status);
            if(code != 0){
                found = 1;
                printf("[PARENT] CHILD %d FOUND THE NUMBER \n", code);
                for(int w= 0; w < PROCESSES; w++){
                    kill(pids[w], SIGKILL);
                    
                }
            }
        }
    }

    if(!found){
        printf("[PARENT] NUMBER WAS NOT FOUND \n");
    }

    return 0;
}
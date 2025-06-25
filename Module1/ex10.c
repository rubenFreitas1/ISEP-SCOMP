#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

#define NUMBER 20
#define PROCESS 5

void fill_array(int *array){
    int used[NUMBER*PROCESS] = {0};
    srand(time(NULL));
    for (int i = 0; i < NUMBER*PROCESS; i++) {
        int num;
        do {
            num = rand() % (100+1 ); 
        } while (used[num] != 0);
        array[i] = num;
        used[num] = 1;
    }
}


int main(){
    int status;
    int found = 0;
    pid_t p[PROCESS];
    int numbers[NUMBER*PROCESS];
    int selectedNumber = 5;

    fill_array(numbers);


    printf("Selected Number: %d \n", selectedNumber);
    for(int i = 0; i < PROCESS; i++){
        p[i] = fork();
        if(p[i] == -1){
            perror("error");
            exit(1);
        }
        if(p[i] == 0){
            int start = i * NUMBER;
            int end = start + NUMBER;
            for(int j = start; j < end; j++){
                if(numbers[j] == selectedNumber){
                    printf("CHILD %d || POSITION %d || NUMBER FOUND %d \n", i+1, j, numbers[j]);
                    exit(i+1);
                }
            }
            exit(0);
        }
    }

    for(int i = 0; i < PROCESS; i++){
        waitpid(p[i], &status, 0);
        if(WIFEXITED(status)){
            int exit = WEXITSTATUS(status);
            if(exit != 0){
                printf("CHILD %d || CHILD PID %d\n", exit, p[i]);
                found = 1;
            }
        }
    }

    if(found == 0){
        printf("Selected Number was not found!\n");
    }

    return 0;
}
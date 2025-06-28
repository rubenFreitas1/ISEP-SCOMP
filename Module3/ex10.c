#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define NUM_CHILDREN 5

int main(){
    int pipes[NUM_CHILDREN + 1][2]; 
    pid_t pids[NUM_CHILDREN];

    for (int i = 0; i <= NUM_CHILDREN; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < NUM_CHILDREN; i++){
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if(pids[i] == 0){
            srand(getpid());
            int my_num = (rand() % 500) + 1;
            int received_num;
            read(pipes[i][0], &received_num, sizeof(int));
            printf("Child %d generated: %d\n", getpid(), my_num);

            if(my_num > received_num){
                received_num = my_num;
            }
            write(pipes[i+1][1], &received_num, sizeof(int));

            for (int j = 0; j <= NUM_CHILDREN; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            exit(EXIT_SUCCESS);
        }
    }

    srand(getpid());
    int parent_num = (rand() % 500) + 1;
    printf("Parent %d generated: %d\n", getpid(), parent_num);

    write(pipes[0][1], &parent_num, sizeof(int));


    int final_max;
    read(pipes[NUM_CHILDREN][0], &final_max, sizeof(int));
    printf("\nParent received final max: %d\n", final_max);

    for (int i = 0; i <= NUM_CHILDREN; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < NUM_CHILDREN; i++) {
        wait(NULL);
    }

    return 0;
}
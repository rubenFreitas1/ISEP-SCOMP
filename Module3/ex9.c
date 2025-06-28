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
    int credit = 20;
    pid_t pid;
    int down[2];
    int up[2];
    int bytes_read = 0;
    int ver = 1;

    if(pipe(down) == -1 || pipe(up) == -1){
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if(pid == -1){
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if(pid == 0){
        close(up[1]);
        close(down[0]);
        srand(time(NULL) + getpid());
        while( (bytes_read = read(up[0], &ver, sizeof(int))) > 0){
            if(ver == 0){
                printf("--- CREDIT ENDED ---\n");
                close(up[1]);
                close(down[1]);
                exit(EXIT_SUCCESS);
            }else if(ver == 1){
                int betNumber = (rand() % 5) + 1;
                write(down[1], &betNumber, sizeof(int)); 
            }
        }
    }

    close(up[0]);
    close(down[1]);
    srand(time(NULL));
    while(ver == 1){
        printf("\n--- CREDIT: %d --- \n", credit);
        int expNumber = (rand() % 5) + 1;
        printf("EXPECTED NUMBER: %d \n", expNumber);
        write(up[1], &ver, sizeof(int));
        int clientNumber;  
        read(down[0], &clientNumber, sizeof(int));
        printf("CLIENT NUMBER: %d \n", clientNumber);
        if(expNumber == clientNumber){
            credit += 10;
        }else{
            credit -= 5;
        }

        if(credit == 0){
            ver = 0;
        }
    }
    close(up[1]);
    close(down[0]);
    write(up[1], &ver, sizeof(int));
    wait(NULL);

    return 0;
}
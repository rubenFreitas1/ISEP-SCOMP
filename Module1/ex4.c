#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

int main(){
    pid_t pid[3];
    pid_t a;
    int f, status;
    for(f = 0; f < 3; f++){
        pid[f] = fork();
        if(pid[f] == -1){
            perror("error:");
            exit(1);
        }
        if(pid[f] == 0){
            if(f == 0){
			    printf("I am the first child!\n");
			    exit(1);
		    }else if(f == 1){
			    printf("I am the second child!\n");
			    exit(2);
		    }else{
			    printf("I am the third child!\n");
			    exit(3);
		    }
        }
    }


    printf("Parent: waiting for second child without blocking...\n");
    do {
        pid_t result = waitpid(pid[1], &status, WNOHANG);
        if (result == 0) {
            printf("Second child still running...\n");
            sleep(1);  // não bloqueia o pai completamente
        } else if (result == -1) {
            perror("waitpid");
            break;
        } else if (WIFEXITED(status)) {
            printf("Second child with pid %d exited with order %d\n", result, WEXITSTATUS(status));
            break;
        }
    } while (1);

    return 0;
}
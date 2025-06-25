#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

char create_twins(pid_t list[2]){
    for(int i = 0; i< 2; i++){
        list[i] = fork();
        if(list[i] == 0){
            if(i == 0){
                return 'a'; 
            }else{
                return 'b';
            }
        }
    }
    return 'p';
}

int main(){
    int status;
    pid_t pids[6];
    int count = 0;
    for(int i = 0; i < 3; i++){
        pid_t list[2];
        char c = create_twins(list);
        if(c == 'a' | c == 'b'){
            printf("PID : %d  ,,,  Parent PID : %d\n", getpid(), getppid());
			exit(c);	
        }
        pids[count] = list[0];
        count++;
        pids[count] = list[1];
        count++;
    }
    for(int i = 0; i < 6; i++){
		pid_t pid = waitpid(pids[i], &status, 0);
		if(WIFEXITED(status)){
			printf("Child PID: %d exited with value: %c\n", pid, WEXITSTATUS(status));
		}
	}
    return 0;
}
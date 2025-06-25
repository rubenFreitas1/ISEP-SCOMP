#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <string.h>


int my_system(const char *command){
    pid_t pid = fork();

    if(pid == -1){
        perror("error");
        exit(EXIT_FAILURE);
    }

    if(pid == 0){
        execl("/bin/sh", "sh", "-c", command, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }else{
        int status;
        waitpid(pid, &status, 0);
        return status;
    }
}



int main(){
    my_system("ls -la");
    my_system("pwd");
    my_system("echo 'Olá mundo'");
    return 0;
}
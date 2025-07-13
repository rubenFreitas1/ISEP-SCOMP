#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>


typedef struct{
    char cmd[32];
    int time;
}command_t;


int main(){
    int status;
    command_t commands[] = {
        {"ls", 5},
        {"date", 3},
        {"whoami", 2}
    };
    int num_commands = sizeof(commands) / sizeof(commands[0]);

    for(int i = 0; i < num_commands; i++){
        pid_t pid = fork();

        if(pid == -1){
            perror("fork failed:");
            exit(EXIT_FAILURE);
        }

        if(pid == 0){
            execlp(commands[i].cmd, commands[i].cmd, (char * )NULL  );
            perror("execlp error:");
            exit(EXIT_FAILURE);
        }

        int elapsed = 0;
        while (elapsed < commands[i].time) {
            pid_t result = waitpid(pid, &status, WNOHANG);
            if (result == pid) {
                printf("Comando '%s' terminou dentro do tempo.\n", commands[i].cmd);
                break;
            }
            sleep(1);
            elapsed++;
        }

        if (elapsed >= commands[i].time) {
            printf("Comando '%s' excedeu o tempo e vai ser terminado.\n", commands[i].cmd);
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
        }
        
    }


}
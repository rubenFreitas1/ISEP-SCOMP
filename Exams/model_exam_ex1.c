#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>

#define SIZE 500
#define CHILDS 5

typedef struct{
    int number;
    char content[SIZE];
}Bilhete;

typedef struct{
    int numberBilhete;
    int scanner;
}Pedido;


int main(){
    int pipes[CHILDS][2];
    int fd[2];
    pid_t pids[CHILDS];
    Bilhete bilhetes[SIZE];
    srand(time(NULL));
    Bilhete bilhete;

    for(int i = 0; i < SIZE; i++){
        bilhetes[i].number = i;
        char localString[SIZE];
        snprintf(localString, SIZE, "informação do bilhete: %d , Válido até dia %d !", bilhetes[i].number, (rand()%24) +1 );
        strcpy(bilhetes[i].content, localString);
    }
    if(pipe(fd) == -1){
        perror("pipe failed fd");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < CHILDS; i++){
        if(pipe(pipes[i]) == -1){
            perror("pipe failed fd");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < CHILDS; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            perror("pids fork failed");
            exit(EXIT_FAILURE);
        }
        if(pids[i] == 0){
            close(fd[0]);
            while(1){
                Pedido pedido;
                pedido.scanner = i;
                pedido.numberBilhete = (rand() % 550 ) + 1;
                write(fd[1], &pedido, sizeof(Pedido));
                read(pipes[i][0],&bilhete , sizeof(Bilhete));
                if(bilhete.number == -1){
                    break;
                }
                printf("[SCANNER %d] INFORMAÇÃO DO BILHETE PEDIDO || NUMBER: %d, CONTENT: %s \n",i+1, bilhete.number, bilhete.content);
            }
            
            for(int i = 0; i < CHILDS;i++){
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
            close(fd[1]);
            exit(1);
        }

    }

    close(fd[1]);

    for(int w = 0; w < CHILDS; w++){
		close(pipes[w][0]);	
	}

    while(1){
        int found = 0;
        Pedido pedido;
        read(fd[0], &pedido, sizeof(Pedido));
        for(int i = 0; i < SIZE; i++){
            if(bilhetes[i].number == pedido.numberBilhete){
                write(pipes[pedido.scanner][1], &bilhetes[i], sizeof(Bilhete));
                found = 1;
            }
        }
        if(found == 0){
            Bilhete bilheteErro;
            bilhete.number = -1;
            write(pipes[pedido.scanner][1], &bilheteErro, sizeof(Bilhete));
            break;
        }
    }

    for(int i = 0; i < CHILDS; i++){
        kill(pids[i],SIGKILL );
    }

    close(fd[0]);
    for(int w = 0; w < CHILDS; w++){
		close(pipes[w][1]);	
	}

    for(int i= 0; i < CHILDS; i++){
		wait(NULL);
	}

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

//a) Não pode ser partilhado pois a lista result do processo filho é uma cópia da lista result do processo pai, ou seja, o processo
//   filho pode alterar os valores mas ao dar print não vai mostrar esses números alterados.

//b) Memória partilhada.


#define NUMBER 100

int main(){
    pid_t a;
    int data[NUMBER];
    int result[NUMBER];
    srand(time(NULL)); 
    for (int i = 0; i < NUMBER; i++) {
        data[i] = (rand() % 10) + 1;
    }
    a = fork();
    if(a == -1){
        perror("error:");
        exit(1);
    }
    if(a == 0){
        for(int i = 0; i < NUMBER/2; i++){
            result[i] = data[i]*4 + 20;
        }
        exit(1);
    }else{
        for(int i = NUMBER/2; i < NUMBER; i++){
            result[i] = data[i]*4 + 20;
        }
    }

    wait(NULL);
    for(int i = 0; i < NUMBER; i++){
        printf("%d \n", result[i]);
    }
    return 0;
}
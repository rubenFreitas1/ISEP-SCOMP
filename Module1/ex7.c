#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>


void M(char *frase){
    printf("%s\n", frase);
}

int main(){
    pid_t p = 0;
    pid_t x = 0;
    for(int i = 0; i < 2; i ++){
        if(x == 0){
           p = fork();
           if(p > 0){
             M("A");
           }else{
             M("B");
             x = fork();
             if(x > 0){
                M("A");
             }
           } 
        }
    }
    return 0;
}